//
// Created by xapulc on 13.11.2020.
//

#ifndef GLADIATORSIMULATION_SIMULATION_H
#define GLADIATORSIMULATION_SIMULATION_H


#include <valarray>
#include <random>
#include "../Gladiator/StrengthVector.h"

class Simulation {
public:
    static StrengthVector simulationForOneTeamWithOneEnemy(double totalStrength,
                                                           int gladiatorNumber,
                                                           const StrengthVector& enemy,
                                                           int generationNumber=10,
                                                           int epochs=10,
                                                           double reshuffleCoefficient=0.5);
private:
    static std::vector<StrengthVector> initialize(double totalStrength,
                                                  int gladiatorNumber,
                                                  int generationNumber,
                                                  std::default_random_engine randomGenerator);
    static void select(std::vector<StrengthVector>& generation,
                       const StrengthVector& enemy);
    static std::vector<StrengthVector> crossbreed(std::vector<StrengthVector> generation,
                                                  std::mt19937 randomGenerator);
    static std::vector<StrengthVector> mutate(std::vector<StrengthVector> generation,
                                              std::mt19937 randomGenerator);
};


#endif //GLADIATORSIMULATION_SIMULATION_H
