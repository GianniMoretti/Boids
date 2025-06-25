cmake_minimum_required(VERSION 3.10)
project(BoidsSimulation)

# Imposta il C++ standard
set(CMAKE_CXX_STANDARD 17)

# Aggiungi l'eseguibile
add_executable(boids main.cpp src/boidRenderer.cpp)

# Trova e linka la libreria SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
target_link_libraries(boids sfml-graphics sfml-window sfml-system)