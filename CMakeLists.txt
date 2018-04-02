cmake_minimum_required(VERSION 3.10)
project(CRI_Course_Project)

add_definitions(-std=c++11 -Wall -pthread)

include_directories(.)

add_executable(CRI_Course_Project
        helper.h handlers.h
        main.cpp)
