cmake_minimum_required(VERSION 3.6)
project(app)

option(PERFECT_OUT "PERFECT_OUT" OFF)
option(MULTITHREAD "MULTITHREAD" OFF)

set(CMAKE_CXX_STANDARD 20)

if (PERFECT_OUT)
    add_definitions(-DPERFECT_OUT)
endif()

add_executable(${PROJECT_NAME} main.cpp impl.cpp dnf.cpp streamData.cpp)

if (MULTITHREAD)
    find_package(Boost REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${Boost})
    add_definitions(-DMULTITHREAD)
endif()
