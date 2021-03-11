all: compile link
compile:
	g++ -Isrc/include -c main.cpp

link:
	g++ main.o -o bin/main -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system

