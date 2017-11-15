CC = g++
CFLAGS = -O3 -std=c++11 -march=native -fopenmp
LIBS = -lOpenCL -lrt
ifndef DEVICE
	DEVICE = CL_DEVICE_TYPE_DEFAULT
endif

CCFLAGS = -D DEVICE=$(DEVICE)

all: othello

othello: gpu.o player.o board.o game.o openings.o heuristicEvaluation.o
	${CC} ${CFLAGS} -o ../othello gpu.o othello.cpp player.o board.o game.o openings.o heuristicEvaluation.o $(LIBS) 

game.o: game.h game.cpp player.o
	${CC} ${CFLAGS} -c game.cpp

board.o: board.h board.cpp
	${CC} ${CFLAGS} -c board.cpp

player.o: external.h player.h player.cpp
	${CC} ${CFLAGS} -c player.cpp 

openings.o: openings.h openings.cpp
	${CC} ${CFLAGS} -c openings.cpp 

heuristicEvaluation.o: external.h heuristicEvaluation.h heuristicEvaluation.cpp
	${CC} ${CFLAGS} -c heuristicEvaluation.cpp

gpu.o: gpu.cpp score_cal.cl
	$(CC) -c $< $(CFLAGS) $(CCFLAGS) -o $@ $(LIBS) 


clean:
	rm ../othello *.o