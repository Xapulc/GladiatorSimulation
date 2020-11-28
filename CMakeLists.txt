cmake_minimum_required(VERSION 3.16)
project(GladiatorSimulation)

set(CMAKE_CXX_STANDARD 20)

add_executable(GladiatorSimulation main.cpp Gladiator/StrengthVector.cpp Gladiator/StrengthVector.h
               Simulation/Simulation.cpp Simulation/Simulation.h
               Simulation/QualityEstimation.cpp Simulation/QualityEstimation.h Simulation/MultiVector.cpp Simulation/MultiVector.h Simulation/MultiIndexGeneric.cpp Simulation/MultiIndexGeneric.h testMultiGame.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(GladiatorSimulation Threads::Threads)