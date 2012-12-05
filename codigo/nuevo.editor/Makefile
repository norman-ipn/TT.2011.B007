Figure.o: Figure.cpp
	g++ -ggdb `pkg-config --cflags opencv` Figure.cpp `pkg-config --libs opencv` -c -g3
Window.o: Window.cpp
	g++ -ggdb `pkg-config --cflags opencv` Window.cpp `pkg-config --libs opencv` -c -g3 
main.o: main.cpp
	g++ -ggdb `pkg-config --cflags opencv` main.cpp `pkg-config --libs opencv` -c -g3 
editor: main.o Window.o Figure.o
	g++ -ggdb `pkg-config --cflags opencv` main.o Figure.o Window.o -o editor `pkg-config --libs opencv` 

clean:
	rm editor
	rm main.o
	rm Window.o
	rm Figure.o

