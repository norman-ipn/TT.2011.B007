#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include"Figure.hpp"

class Window{
	public:
		int width;
		int heigth;
	public:
		IplImage* image;
		int followmousemove;
		char task;
	public:
		Window(int,int);
		void show(void);
		void refresh();
		bool isTask();
};


#endif
