cmake_minimum_required(VERSION 3.6)
project(app)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp impl.cpp dnf.cpp streamData.cpp)
