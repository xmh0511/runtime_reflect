cmake_minimum_required(VERSION 3.10)
project(runtime_reflection)

set(CMAKE_CXX_STANDARD 17)

add_executable(runtime_reflection main.cpp rttr.hpp)