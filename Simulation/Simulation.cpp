//
// Created by xapulc on 13.11.2020.
//


#include "Simulation.h"

StrengthVector Simulation::simulationForOneTeamWithOneEnemy(const double totalStrength,
                                                            const int gladiatorNumber,
                                                            const StrengthVector &enemy,
                                                            const int generationNumber,
                                                            const int epochs,
                                                            const double mutationCoefficient,
                                                            const int threadsNumber) {
    std::default_random_engine randomGenerator;
    std::random_device rd;
    std::mt19937 uniformGenerator(rd());
    auto generation = initialize(totalStrength, gladiatorNumber, generationNumber, randomGenerator, threadsNumber);
    auto selectedNumber = std::trunc(generation.size() * mutationCoefficient);

    for (int epoch = 0; epoch < epochs; epoch++) {
        selectOneTeam(generation, enemy, threadsNumber);
        auto mutationTeams = mutate(std::vector<StrengthVector>(generation.begin(), generation.begin() + selectedNumber),
                                    uniformGenerator,
                                    threadsNumber);

        auto crossbredTeams = crossbreed(std::vector<StrengthVector>(generation.begin(), generation.begin() + selectedNumber),
                                         generationNumber - selectedNumber,
                                         uniformGenerator,
                                         threadsNumber);

        for (int i = 0; i < selectedNumber; i++) {
            generation[i] = mutationTeams[i];
        }

        for (int i = selectedNumber; i < generationNumber; i++) {
            generation[i] = crossbredTeams[i - selectedNumber];
        }
    }

    selectOneTeam(generation, enemy, threadsNumber);

    std::cout << "***** TOP *****" << std::endl;
    for (int i = 0; i < 7; i++) {
        std::cout << "Probability of win: " << QualityEstimation::probabilityOfWinLeftTeam(generation[i], enemy) << "; ";
        generation[i].print();
    }
    return generation[0];
}

std::vector<StrengthVector> Simulation::initialize(const double totalStrength,
                                                   const int gladiatorNumber,
                                                   const int generationNumber,
                                                   std::default_random_engine randomGenerator,
                                                   const int threadsNumber) {
    auto initialGeneration = std::vector<StrengthVector>(generationNumber);
    std::exponential_distribution<double> distribution(1);
    auto futures = std::vector<std::future<StrengthVector>>((unsigned long) threadsNumber);

    auto createTeam = [gladiatorNumber, &distribution, &randomGenerator, totalStrength]() {
        auto team = StrengthVector(gladiatorNumber);
        double randomValuesSum = 0;
        for (int j = 0; j < gladiatorNumber; j++) {
            auto strength = distribution(randomGenerator);
            randomValuesSum += strength;
            team[j] = strength;
        }
        team *= totalStrength / randomValuesSum;
        return team;
    };
    for (int i = 0; i < generationNumber; i++) {
        futures[i % threadsNumber] = std::async(std::launch::async, createTeam);
        if (((i + 1) % threadsNumber == 0) || (i + 1 == generationNumber)) {
            auto startIndex = threadsNumber * (i / threadsNumber);
            for (int k = 0; k < std::min(threadsNumber, generationNumber - startIndex); k++) {
                initialGeneration[startIndex + k] = futures[k].get();
            }
        }
    }
    return initialGeneration;
}

void Simulation::selectOneTeam(std::vector<StrengthVector>& generation,
                               const StrengthVector& enemy,
                               const int threadsNumber) {
    auto qualityFunction = [enemy, threadsNumber](StrengthVector &firstLeftTeam, StrengthVector &secondLeftTeam) {
        if (threadsNumber > 1) {
            auto futureLeftTeam = std::async(std::launch::async, QualityEstimation::probabilityOfWinLeftTeam, firstLeftTeam, enemy);
            auto futureRightTeam = std::async(std::launch::async, QualityEstimation::probabilityOfWinLeftTeam, secondLeftTeam, enemy);
            return futureLeftTeam.get() > futureRightTeam.get();
        } else {
            return QualityEstimation::probabilityOfWinLeftTeam(firstLeftTeam, enemy) > QualityEstimation::probabilityOfWinLeftTeam(secondLeftTeam, enemy);
        }
    };
    std::sort(generation.begin(), generation.end(), qualityFunction);
}

std::vector<StrengthVector> Simulation::crossbreed(std::vector<StrengthVector> generation,
                                                   const int crossbredGenerationNumber,
                                                   std::mt19937 randomGenerator,
                                                   const int threadsNumber) {
    auto crossbredGeneration = std::vector<StrengthVector>(crossbredGenerationNumber);
    std::uniform_int_distribution<> firstItemDistribution(0, generation.size()-1);
    std::uniform_int_distribution<> secondItemDistribution(0, generation.size()-2);
    std::uniform_real_distribution<> linearCoefficientDistribution(0, 1);
    auto futures = std::vector<std::future<StrengthVector>>((unsigned long) threadsNumber);

    auto createTeam = [generation, &firstItemDistribution, &secondItemDistribution,
                       &linearCoefficientDistribution, &randomGenerator]() {
        auto firstTeamIndex = firstItemDistribution(randomGenerator);
        auto secondTeamIndex = secondItemDistribution(randomGenerator);
        secondTeamIndex += (secondTeamIndex >= firstTeamIndex) ? 1 : 0;
        auto alpha = linearCoefficientDistribution(randomGenerator);

        return generation[firstTeamIndex] * alpha + generation[secondTeamIndex] * (1 - alpha);
    };

    for (int i = 0; i < crossbredGenerationNumber; i++) {
        futures[i % threadsNumber] = std::async(std::launch::async, createTeam);
        if (((i + 1) % threadsNumber == 0) || (i + 1 == crossbredGenerationNumber)) {
            auto startIndex = threadsNumber * (i / threadsNumber);
            for (int k = 0; k < std::min(threadsNumber, crossbredGenerationNumber - startIndex); k++) {
                crossbredGeneration[k + startIndex] = futures[k].get();
            }
        }
    }
    return crossbredGeneration;
}

std::vector<StrengthVector> Simulation::mutate(std::vector<StrengthVector> generation,
                                               std::mt19937 randomGenerator,
                                               const int threadsNumber) {
    auto mutatedGeneration = std::vector<StrengthVector>(generation.size());
    std::uniform_int_distribution<> firstMutatedGladiatorDistribution(0, generation[0].getLength()-1);
    std::uniform_int_distribution<> secondMutatedGladiatorDistribution(0, generation[0].getLength()-2);
    std::uniform_real_distribution<> linearCoefficientDistribution(0, 1);
    auto futures = std::vector<std::future<StrengthVector>>((unsigned long) threadsNumber);

    auto createTeam = [generation, &firstMutatedGladiatorDistribution, &secondMutatedGladiatorDistribution,
            &linearCoefficientDistribution, &randomGenerator](const int i) {
        auto firstGladiatorIndex = firstMutatedGladiatorDistribution(randomGenerator);
        auto secondGladiatorIndex = secondMutatedGladiatorDistribution(randomGenerator);
        secondGladiatorIndex += (secondGladiatorIndex >= firstGladiatorIndex) ? 1 : 0;
        auto alpha = linearCoefficientDistribution(randomGenerator);

        auto mutatedTeam = generation[i];

        auto sumPairStrength = mutatedTeam[firstGladiatorIndex] + mutatedTeam[secondGladiatorIndex];
        mutatedTeam[firstGladiatorIndex] = sumPairStrength * alpha;
        mutatedTeam[secondGladiatorIndex] = sumPairStrength * (1 - alpha);

        return mutatedTeam;
    };

    for (int i = 0; i < generation.size(); i++) {
        futures[i % threadsNumber] = std::async(std::launch::async, createTeam, i);
        if (((i + 1) % threadsNumber == 0) || (i + 1 == generation.size())) {
            auto startIndex = threadsNumber * (i / threadsNumber);
            for (int k = 0; k < std::min(threadsNumber, int(generation.size()) - startIndex); k++) {
                mutatedGeneration[k + startIndex] = futures[k].get();
            }
        }
    }
    return mutatedGeneration;
}

std::vector<StrengthVector> Simulation::simulationTeams(const std::vector<double> totalStrengths,
                                                        const std::vector<int> gladiatorNumbers,
                                                        const int generationNumber,
                                                        const int epochs,
                                                        const double mutationCoefficient,
                                                        const int threadsNumber) {
    std::default_random_engine randomGenerator;
    std::random_device rd;
    std::mt19937 uniformGenerator(rd());
    auto generations = std::vector<std::vector<StrengthVector>>(totalStrengths.size());
    auto selectedNumbers = std::vector<int>(totalStrengths.size());
    for (int i = 0; i < totalStrengths.size(); i++) {
        generations[i] = initialize(totalStrengths[i], gladiatorNumbers[i], generationNumber, randomGenerator, threadsNumber);
        selectedNumbers[i] = std::trunc(generations[i].size() * mutationCoefficient);
    }

    for (int epoch = 0; epoch < epochs; epoch++) {
        selectSomeTeams(generations, threadsNumber);
        for (int j = 0; j < totalStrengths.size(); j++) {
            auto mutationTeams = mutate(std::vector<StrengthVector>(generations[j].begin(), generations[j].begin() + selectedNumbers[j]),
                                        uniformGenerator,
                                        threadsNumber);

            auto crossbredTeams = crossbreed(std::vector<StrengthVector>(generations[j].begin(), generations[j].begin() + selectedNumbers[j]),
                                             generationNumber - selectedNumbers[j],
                                             uniformGenerator,
                                             threadsNumber);

            for (int i = 0; i < selectedNumbers[i]; i++) {
                generations[j][i] = mutationTeams[i];
            }

            for (int i = selectedNumbers[j]; i < generationNumber; i++) {
                generations[j][i] = crossbredTeams[i - selectedNumbers[j]];
            }
        }
    }

    auto probabilities = selectSomeTeams(generations, threadsNumber);

    std::cout << "***** TOP *****" << std::endl << std::endl << std::endl;
    for (int i = 0; i < totalStrengths.size(); i++) {
        std::cout << std::endl << "***** Team " << i << " *****" << std::endl;
        for (int j = 0; j < 3; j++) {
            std::cout << "Probability of win: " << probabilities[i][j] << std::endl;
            generations[i][j].print();
        }
    }

    auto topTeams = std::vector<StrengthVector>(totalStrengths.size());
    for (int i = 0; i < totalStrengths.size(); i++) {
        topTeams[i] = generations[i][0];
    }

    return topTeams;
}

template <typename A, typename B>
void zip(
        const std::vector<A> &a,
        const std::vector<B> &b,
        std::vector<std::pair<A,B>> &zipped)
{
    for(size_t i=0; i<a.size(); ++i)
    {
        zipped.push_back(std::make_pair(a[i], b[i]));
    }
}

template <typename A, typename B>
void unzip(
        const std::vector<std::pair<A, B>> &zipped,
        std::vector<A> &a,
        std::vector<B> &b)
{
    for(size_t i = 0; i < a.size(); i++)
    {
        a[i] = zipped[i].first;
        b[i] = zipped[i].second;
    }
}

std::vector<std::vector<double>> Simulation::selectSomeTeams(std::vector<std::vector<StrengthVector>>& generations,
                                                             const int threadsNumber) {
    auto probabilitiesOfWin = std::vector<std::vector<double>>(generations.size());
    auto dimensionalGeneric = std::vector<int>(generations.size());

    for (int i = 0; i < generations.size(); i++) {
        probabilitiesOfWin[i] = std::vector<double>(generations[i].size());
        dimensionalGeneric[i] = generations[i].size();
        for (int j = 0; j < generations[i].size(); j++) {
            probabilitiesOfWin[i][j] = 0;
        }
    }
    auto multiIndexGeneric = MultiIndexGeneric(dimensionalGeneric);
    auto futures = std::vector<std::future<int>>((unsigned long) threadsNumber);

    auto calculateProbabilitiesWin = [&probabilitiesOfWin, &generations](const std::vector<int>& k) {
        auto teams = std::vector<StrengthVector>(generations.size());
        for (int i = 0; i < generations.size(); i++) {
            teams[i] = generations[i][k[i]];
        }

        auto probabilities = QualityEstimation::probabilitiesOfWin(teams);
        for (int i = 0; i < generations.size(); i++) {
            probabilitiesOfWin[i][k[i]] += probabilities[i];
        }
        return 0;
    };

    int currentFutures = 0;
    while (true) {
        auto k = multiIndexGeneric.getIndex();
        futures[currentFutures++] = std::async(std::launch::async, calculateProbabilitiesWin, k);
        auto isContinue = multiIndexGeneric.next();

        if ((currentFutures == threadsNumber) || (!isContinue)) {
            for (int i = 0; i < currentFutures; i++) {
                futures[i].get();
            }
            currentFutures = 0;
        }

        if (!isContinue) {
            break;
        }
    }

    int generationsDimensionalProduct = 1;
    for (auto &generation: generations) {
        generationsDimensionalProduct *= generation.size();
    }

    for (auto &probability: probabilitiesOfWin) {
        auto denomination = generationsDimensionalProduct / probability.size();
        for (auto &el: probability) {
            el /= denomination;
        }
    }

    for (int i = 0; i < generations.size(); i++) {
        std::vector<std::pair<StrengthVector, double>> zipped;
        zip(generations[i], probabilitiesOfWin[i], zipped);

        std::sort(zipped.begin(), zipped.end(),
                  [&](const auto& a, const auto& b) {
                      return a.second > b.second;
                  });

        unzip(zipped, generations[i], probabilitiesOfWin[i]);
    }

    return probabilitiesOfWin;
}
