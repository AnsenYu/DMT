G=-g

simulator:simulator.o simu.o stdafx.o
	g++ $G -o simulator simulator.o simu.o stdafx.o
simulator.o: simulator.cpp stdafx.h simu.h
	g++ -c $G simulator.cpp
simu.o: simu.cpp stdafx.h simu.h
	g++ -c $G simu.cpp
stdafx.o: stdafx.cpp stdafx.h
	g++ -c $G stdafx.cpp

clean:
	rm simulator simulator.o simu.o stdafx.o
