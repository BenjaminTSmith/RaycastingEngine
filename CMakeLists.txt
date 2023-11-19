cmake_minimum_required(VERSION 3.26)
project(raycasting)

set(CMAKE_CXX_STANDARD 17)

add_executable(raycasting main.cpp
        functions.h)
find_package(SFML COMPONENTS system window graphics audio CONFIG REQUIRED)
target_link_libraries(raycasting PRIVATE sfml-system sfml-network sfml-graphics sfml-window sfml-audio)
