maximizeScore: Common.o Roll.o Scorer.o Genetic.o main.cpp 
	g++ -o maximizeScore -g main.cpp Roll.o Common.o Scorer.o Genetic.o

Roll.o: Roll.cpp Roll.h Common.h Scorer.h
	g++ -c Roll.cpp

Common.o: Common.cpp Common.h
	g++ -c Common.cpp

Scorer.o: Scorer.cpp Scorer.h Common.h
	g++ -c Scorer.cpp

Genetic.o: Genetic.cpp Genetic.h Common.h Roll.h Scorer.h
	g++ -c Genetic.cpp
