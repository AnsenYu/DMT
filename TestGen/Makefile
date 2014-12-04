bin=TestGen
obj=DijkstraShortestPathAlg.o Graph.o possion.o stdafx.o TestGen.o YenTopKShortestPathsAlg.o Floyd.o
G=-g
$(bin):$(obj)
	g++ $G -o $(bin) $(obj)
DijkstraShortestPathAlg.o: DijkstraShortestPathAlg.cpp DijkstraShortestPathAlg.h GraphElements.h stdafx.h Graph.h
	g++ $G -c DijkstraShortestPathAlg.cpp
Graph.o: Graph.cpp stdafx.h Graph.h GraphElements.h
	g++ $G -c Graph.cpp
possion.o: possion.cpp stdafx.h possion.h
	g++ $G -c possion.cpp
stdafx.o: stdafx.cpp stdafx.h
	g++ $G -c stdafx.cpp
Floyd.o: Floyd.cpp Floyd.h stdafx.h
	g++ $G -c Floyd.cpp
TestGen.o: TestGen.cpp stdafx.h GraphElements.h Graph.h DijkstraShortestPathAlg.h YenTopKShortestPathsAlg.h possion.h conf.h Floyd.h
	g++ $G -c TestGen.cpp 
YenTopKShortestPathsAlg.o: YenTopKShortestPathsAlg.cpp YenTopKShortestPathsAlg.h DijkstraShortestPathAlg.h stdafx.h GraphElements.h Graph.h
	g++ $G -c YenTopKShortestPathsAlg.cpp

clean:
	rm $(bin) $(obj)
