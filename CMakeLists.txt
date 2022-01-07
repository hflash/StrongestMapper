cmake_minimum_required(VERSION 3.14)
project(StrongestMapper)
set(CMAKE_CXX_STANDARD 14)
add_executable(StrongestMapper main.cpp src/parser/QASMparser.cpp src/parser/QASMparser.h src/search/SearchNode.cpp src/search/SearchNode.h src/parser/Environment.cpp src/parser/Environment.h src/queue/Queue.h src/filter/Filter.h src/queue/DefaultQueue.hpp src/filter/HashFilter.hpp src/expander/Expander.h src/expander/DefaultExpander.cpp src/expander/DefaultExpander.h src/filter/HashFilter2.hpp)