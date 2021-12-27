cmake_minimum_required(VERSION 3.14)
project(StrongestMapper)
set(CMAKE_CXX_STANDARD 14)
add_executable(StrongestMapper main.cpp src/parser/QASMparser.cpp src/parser/QASMparser.h src/parser/Environment.cpp src/parser/Environment.h src/queue/Queue.h src/filter/Filter.h src/queue/DefaultQueue.cpp src/queue/DefaultQueue.h)