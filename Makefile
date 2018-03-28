all: main

COMPILER = clang++

main: main.c
	$(COMPILER) main.c -Wall -pthread -std=c++11