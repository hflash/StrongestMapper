cmake_minimum_required(VERSION 3.19)
project(StrongestMapper)
set(CMAKE_CXX_STANDARD 14)
add_executable(StrongestMapper main.cpp src/parser/QASMparser.cpp src/parser/QASMparser.h src/daggenerator/Dag.cpp src/daggenerator/Dag.h)