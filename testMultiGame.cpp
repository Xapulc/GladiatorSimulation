//
// Created by xapulc on 15.11.2020.
//

#include <chrono>
#include "Simulation/Simulation.h"

int main() {
    auto totalStrengths = std::vector<double>{1, 1.2, 1.4};
    auto gladiatorNumbers = std::vector<int>{5, 6, 7};
    int threadsNumber = 8;

    if ((threadsNumber <= 0) || (threadsNumber > std::thread::hardware_concurrency())) {
        std::cout << "There is wrong number" << threadsNumber << std::endl;
        exit(-1);
    }

    auto start_time =  std::chrono::system_clock::now();
    auto optimalTeams = Simulation::simulationTeams(totalStrengths,
                                                    gladiatorNumbers,
                                                    75,
                                                    75,
                                                    0.95,
                                                    threadsNumber);
    auto end_time =  std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end_time - start_time;
    std::cout << "Time work: " << diff.count() << " s" << std::endl;
    return 0;
}
