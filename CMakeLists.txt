cmake_minimum_required(VERSION 3.10)
project(CRI_Course_Project)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(CRI_Course_Project
        helper.h
        main.cpp)
