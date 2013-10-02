calc.exe: main.o
	g++ -o calc.exe main.o
main.o: main.cpp
	g++ -std=c++0x -c main.cpp -g
clean:
	del *.o *.exe