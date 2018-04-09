all: main

COMPILER = clang++

main: main.cpp
	$(COMPILER) main.cpp -Wall -pthread -std=c++11 -o hw3.out
