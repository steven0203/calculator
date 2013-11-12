calc.exe: main.o
	g++ -o calc main.o
main.o: main.cpp
	g++ -std=c++0x -c main.cpp -g
clean:
	rm *.o calc
