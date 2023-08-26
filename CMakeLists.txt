cmake_minimum_required(VERSION 3.27)
project(cloxpp)
set(CMAKE_CXX_STANDARD 17)
add_executable(jlox src/main.cpp src/treewalkinterpreter.cpp)
