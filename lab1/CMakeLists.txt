cmake_minimum_required(VERSION 3.6)
project(app)

option(PERFECT_OUT "PERFECT_OUT" OFF)

set(CMAKE_CXX_STANDARD 20)

if (PERFECT_OUT)
    add_definitions(-DPERFECT_OUT)
endif()

find_package(Boost REQUIRED)

add_executable(${PROJECT_NAME} main.cpp impl.cpp dnf.cpp streamData.cpp)

target_link_libraries(${PROJECT_NAME} ${Boost})
