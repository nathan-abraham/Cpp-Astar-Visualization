all: compile link
compile:
	g++ -Irsrc/include -c src/main.cpp

link:
	g++ src/main.o -o bin/main -Lrsrc/lib -lsfml-graphics -lsfml-window -lsfml-system

