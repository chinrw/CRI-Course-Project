cmake_minimum_required(VERSION 3.10)
project(CRI_Course_Project)


ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    # Other flags
)

include_directories(.)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -g -Wall -pthread")

add_executable(CRI_Course_Project
        helper.h handlers.h
        main.cpp)
