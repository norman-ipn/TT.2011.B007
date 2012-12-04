#ifndef __FIGURE_HPP__

#define __FIGURE_HPP__

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>

class Figure{
	public:
		CvPoint fp;
		CvPoint lp;
		int drawtype;
		CvScalar color;
		int thickness;
		char task;
	public:
		Figure(int, int, int);
		void draw(IplImage*);
		void showInfo();
		bool selected(int,int);
		void change();
		void move(int,int);
		void setColor(int, int, int);
		void setZoom(double);
	private:
		void changePoints();
		int getPartOfEquLine(int, int, int, int);
		int getPartOfEquCircle(int, int, int, int);
		int getEquEllipse(int, int, int, int);
		int getHalfOf(char);
		void move();
		
};

#endif


