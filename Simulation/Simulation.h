//
// Created by xapulc on 13.11.2020.
//

#ifndef GLADIATORSIMULATION_SIMULATION_H
#define GLADIATORSIMULATION_SIMULATION_H


#include <thread>
#include <future>
#include <valarray>
#include <random>
#include "../Gladiator/StrengthVector.h"
#include "QualityEstimation.h"

class Simulation {
public:
    static StrengthVector simulationForOneTeamWithOneEnemy(double totalStrength,
                                                           int gladiatorNumber,
                                                           const StrengthVector& enemy,
                                                           int generationNumber=10,
                                                           int epochs=10,
                                                           double mutationCoefficient=0.5,
                                                           int threadsNumber=3);
    static std::vector<StrengthVector> simulationTeams(std::vector<double> totalStrengths,
                                                       std::vector<int> gladiatorNumbers,
                                                       int generationNumber=10,
                                                       int epochs=10,
                                                       double mutationCoefficient=0.5,
                                                       int threadsNumber=3);
private:
    static std::vector<StrengthVector> initialize(double totalStrength,
                                                  int gladiatorNumber,
                                                  int generationNumber,
                                                  std::default_random_engine randomGenerator,
                                                  int threadsNumber);
    static void selectOneTeam(std::vector<StrengthVector>& generation,
                              const StrengthVector& enemy,
                              int threadsNumber);
    static std::vector<StrengthVector> crossbreed(std::vector<StrengthVector> generation,
                                                  int crossbredGenerationNumber,
                                                  std::mt19937 randomGenerator,
                                                  int threadsNumber);
    static std::vector<StrengthVector> mutate(std::vector<StrengthVector> generation,
                                              std::mt19937 randomGenerator,
                                              int threadsNumber);

    static std::vector<std::vector<double>> selectSomeTeams(std::vector<std::vector<StrengthVector>> &generations,
                                                            int threadsNumber);
};


#endif //GLADIATORSIMULATION_SIMULATION_H
