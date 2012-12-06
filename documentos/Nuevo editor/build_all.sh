#!/bin/sh

echo "compiling Figure.cpp"
    g++ -ggdb `pkg-config --cflags opencv` Figure.cpp `pkg-config --libs opencv` -c -g3 ;
echo "compiling Window.cpp"
    g++ -ggdb `pkg-config --cflags opencv` Window.cpp `pkg-config --libs opencv` -c -g3 ;
echo "compiling main.cpp"
    g++ -ggdb `pkg-config --cflags opencv` main.cpp Figure.o Window.o -o editor `pkg-config --libs opencv` ;

