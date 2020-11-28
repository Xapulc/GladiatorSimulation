//
// Created by xapulc on 13.11.2020.
//

#include "Simulation.h"

double probabilityOfWinLeftTeam(const StrengthVector& leftTeam, const StrengthVector& rightTeam) {
    /**
     * Вероятность выживания фиксированного количества гладиаторов у каждой команды.
     *
     * Рассматриваются 2 команды гладиаторов с силами `leftTeam` и `rightTeam`.
     * Гладиаторы в этих командах сражаются последовательно до смерти.
     * То есть первыми встречаются a_1 и b_1, затем победитель
     * сражения встречается со вторым гладиатором команды противника и так далее.
     *
     * В возвращаемом методе векторе перечислены сначала
     * вероятности выжить `k` гладиаторам первой команды,
     * где 1 <= `k` <= `m` (количество гладиаторов в первой команде),
     * а заметм перечислены вероятности выжить `l` гладиаторам второй команды,
     * где 1 <= 'l' <= 'n' (количество гладиаторов во второй команде).
     *
     * Вероятность того, что выживет ровно `m-k` гладиаторов у первой команды можно посчитать следующим образом.
     * Положим A_k := \sum_{j=1}^k a_j X_j, B_l := \sum_{i=1}^l b_i Y_i.
     * Согласно работе Каминского вероятность того,
     * что останется в живых `m-k` гладиаторов у первой команды,
     * равна P(A_k < B_n < A_{k+1}) = P(B_n > A_k) - P(B_n > A_{k+1}).
     * Вероятности вида
     *    P(A_k > B_l) = p_{k,l}, где k = m или l = n, (1)
     * находятся итерационным алгоритмом, используя соотношение
     * (b_i / (a_j + b_i)) * p_{j-1,i} + (a_j / (a_j + b_i)) * p_{j,i-1} = p_{j,i}. (2)
     *
     * Объём памяти можно сэкономить на том, что на самом деле нужно хранить
     * два вектора: вектор `winLeftWithFullTeam` := (p_{m,1}, ..., p_{m,n})
     * и вектор `curWinLeft` := (p_{1,i}, p_{2,i}, ..., p_{j-1,i}, p_{j,i-1}, p_{j+1,i-1}, ..., p_{m,i-1}).
     * Первый будет нужен для нахождения вероятностей вида (1) при k = m,
     * второй по правилу (2) и начальным состоянием p_{j,0} = 1, p_{0,i} = 0
     * позволяет вычислить все элементы вида p_{j,n}.
     */
    auto m = leftTeam.getLength();
    auto n = rightTeam.getLength();
    std::vector<double> curWinLeft(m);
    std::fill(curWinLeft.begin(), curWinLeft.end(), 1.0);

    for (int i = 0; i < n; i++) {
        curWinLeft[0] = leftTeam[0] * curWinLeft[0] / (leftTeam[0] + rightTeam[i]);
        for (int j = 1; j < m; j++) {
            curWinLeft[j] = (rightTeam[i] * curWinLeft[j-1] + leftTeam[j] * curWinLeft[j])
                            / (leftTeam[j] + rightTeam[i]);
        }
    }

    return curWinLeft[m-1];
}

StrengthVector Simulation::simulationForOneTeamWithOneEnemy(const double totalStrength,
                                                            const int gladiatorNumber,
                                                            const StrengthVector &enemy,
                                                            const int generationNumber,
                                                            const int epochs,
                                                            const double reshuffleCoefficient) {
    std::default_random_engine randomGenerator;
    std::random_device rd;
    std::mt19937 uniformGenerator(rd());
    auto generation = initialize(totalStrength, gladiatorNumber, generationNumber, randomGenerator);
    auto selectedNumber = std::trunc(reshuffleCoefficient * generation.size());

    for (int epoch = 0; epoch < epochs; epoch++) {
        select(generation, enemy);
//        generation[0].print();
        auto crossbredTeams = crossbreed(std::vector<StrengthVector>(generation.begin(), generation.begin() + selectedNumber),
                                         uniformGenerator);

        auto mutationTeams = mutate(std::vector<StrengthVector>(generation.begin() + selectedNumber, generation.end()),
                                    uniformGenerator);

        for (int i = 0; i < selectedNumber; i++) {
            generation[i] = crossbredTeams[i];
        }

        for (int i = selectedNumber; i < generationNumber; i++) {
            generation[i] = mutationTeams[i - selectedNumber];
        }
    }

    select(generation, enemy);

    for (int i = 0; i < generationNumber; i++) {
        std::cout << "Probability of win: " << probabilityOfWinLeftTeam(generation[i], enemy) << " ";
        generation[i].print();
    }
    return generation[0];
}

std::vector<StrengthVector> Simulation::initialize(const double totalStrength,
                                                   const int gladiatorNumber,
                                                   const int generationNumber,
                                                   std::default_random_engine randomGenerator) {
    auto initialGeneration = std::vector<StrengthVector>(generationNumber);
    std::exponential_distribution<double> distribution(1);

    for (int i = 0; i < generationNumber; i++) {
        auto team = StrengthVector(gladiatorNumber);
        double randomValuesSum = 0;
        for (int j = 0; j < gladiatorNumber; j++) {
            auto strength = distribution(randomGenerator);
            randomValuesSum += strength;
            team[j] = strength;
        }
        team *= totalStrength / randomValuesSum;
        initialGeneration[i] = team;
    }
    return initialGeneration;
}

void Simulation::select(std::vector<StrengthVector>& generation,
                        const StrengthVector& enemy) {
    auto qualityFunction = [enemy](StrengthVector &firstLeftTeam, StrengthVector &secondLeftTeam) {
        return probabilityOfWinLeftTeam(firstLeftTeam, enemy) > probabilityOfWinLeftTeam(secondLeftTeam, enemy);
    };
    std::sort(generation.begin(), generation.end(), qualityFunction);
}

std::vector<StrengthVector> Simulation::crossbreed(std::vector<StrengthVector> generation,
                                                   std::mt19937 randomGenerator) {
    auto crossbredGeneration = std::vector<StrengthVector>(generation.size());
    std::uniform_int_distribution<> firstItemDistribution(0, generation.size()-1);
    std::uniform_int_distribution<> secondItemDistribution(0, generation.size()-2);
    std::uniform_real_distribution<> linearCoefficientDistribution(0, 1);

    for (int i = 0; i < generation.size(); i++) {
        auto firstTeamIndex = firstItemDistribution(randomGenerator);
        auto secondTeamIndex = secondItemDistribution(randomGenerator);
        secondTeamIndex += (secondTeamIndex >= firstTeamIndex) ? 1 : 0;
        auto alpha = linearCoefficientDistribution(randomGenerator);

        crossbredGeneration[i] = generation[firstTeamIndex] * alpha + generation[secondTeamIndex] * (1 - alpha);
    }
    return crossbredGeneration;
}

std::vector<StrengthVector> Simulation::mutate(std::vector<StrengthVector> generation,
                                               std::mt19937 randomGenerator) {
    auto mutatedGeneration = std::vector<StrengthVector>(generation.size());
    std::uniform_int_distribution<> firstMutatedGladiatorDistribution(0, generation[0].getLength()-1);
    std::uniform_int_distribution<> secondMutatedGladiatorDistribution(0, generation[0].getLength()-2);
    std::uniform_real_distribution<> linearCoefficientDistribution(0, 1);

    for (int i = 0; i < generation.size(); i++) {
        auto firstGladiatorIndex = firstMutatedGladiatorDistribution(randomGenerator);
        auto secondGladiatorIndex = secondMutatedGladiatorDistribution(randomGenerator);
        secondGladiatorIndex += (secondGladiatorIndex >= firstGladiatorIndex) ? 1 : 0;
        auto alpha = linearCoefficientDistribution(randomGenerator);

        mutatedGeneration[i] = generation[i];

        auto sumPairStrength = mutatedGeneration[i][firstGladiatorIndex] + mutatedGeneration[i][secondGladiatorIndex];
        mutatedGeneration[i][firstGladiatorIndex] = sumPairStrength * alpha;
        mutatedGeneration[i][secondGladiatorIndex] = sumPairStrength * (1 - alpha);
    }
    return mutatedGeneration;
}
