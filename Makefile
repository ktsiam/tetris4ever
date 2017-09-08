#Author: Kostas Tsiampouris, Aug-2017

CXX      = clang++
CXXFLAGS = -std=c++11 -O3
LDLFLAGS = -pthread -lpthread

all: tetris4ever

tetris4ever: main.o
	${CXX} -o	tetris4ever	board.o	ai.o	main.o

main.o:  main.cpp ai.o board.o
ai.o:	 ai.cpp ai.h evaluation.h board.o
board.o: board.cpp board.h piece.h


clean:
	rm -f *.o
	rm -f tetris4ever

