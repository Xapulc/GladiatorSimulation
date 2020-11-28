#include <chrono>
#include "Simulation/Simulation.h"

int test() {
    double firstTeamTotalStrength = 1;
    double secondTeamTotalStrength = 1.12;
    int firstTeamGladiatorsNumber = 6;
    int secondTeamGladiatorsNumber = 100;
    int threadsNumber = 6;

    if ((threadsNumber <= 0) || (threadsNumber > std::thread::hardware_concurrency())) {
        std::cout << "There is wrong number" << threadsNumber << std::endl;
        exit(-1);
    }

    auto secondTeam = StrengthVector(secondTeamGladiatorsNumber);
    for (int i = 0; i < secondTeamGladiatorsNumber; i++) {
        secondTeam[i] = secondTeamTotalStrength / secondTeamGladiatorsNumber;
    }
    auto start_time =  std::chrono::system_clock::now();
    auto optimalFirstTeam = Simulation::simulationForOneTeamWithOneEnemy(firstTeamTotalStrength,
                                                                         firstTeamGladiatorsNumber,
                                                                         secondTeam,
                                                                         250,
                                                                         280,
                                                                         0.95,
                                                                         threadsNumber);
    auto end_time =  std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end_time - start_time;
    std::cout << "Time work: " << diff.count() << " s" << std::endl;
    return 0;
}
