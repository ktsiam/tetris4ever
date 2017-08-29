#Author: Kostas Tsiampouris, Aug-2017

CXX      = clang++
CXXFLAGS = -std=c++11 -O3 -Wall -Wextra #-Weverything
LDLFLAGS = -pthread -lpthread

all: tetris4ever


tetris4ever:	piece.h	board.h evaluation.h board.o ai.h ai.o main.o
	${CXX} -o	tetris4ever	board.o	ai.o	main.o


clean:
	rm -f *.o
	rm -f tetris4ever

