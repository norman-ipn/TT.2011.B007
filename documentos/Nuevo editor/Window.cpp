#include"Window.hpp"
#include <string> 

Window::Window(int width, int heigth)
{
	
	this->image=cvCreateImage(cvSize(width,heigth),8,3);
	cvZero(this->image);
	cvRectangle(this->image,cvPoint(0,0),cvPoint(width,heigth),CV_RGB(255,255,255),-1);	
	
	this->followmousemove=0;
	this->width=width;
	this->heigth=heigth;
	
	cvNamedWindow("2011-B007",CV_WINDOW_AUTOSIZE);
	//cvResizeWindow(name,width,heigth);
	
}

void Window::refresh()
{
		cvZero(image);
		cvRectangle(image,cvPoint(0,0),cvPoint(width,heigth),CV_RGB(255,255,255),-1);	
}

bool Window::isTask()
{
	switch(task)
	{
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':return true;break;
	}
		
	return false;
}

void Window::show(void)
{
	char tool;
	while(1)
	{
		tool = cvWaitKey(10);		
		
		cvShowImage("2011-B007",this->image);
		if( tool == 27 || tool == 'q' || tool == 'Q' ) // 'ESC', q o Q
			break;
		
	}
	cvReleaseImage( &this->image );
	cvDestroyAllWindows();//Destruimos todas las ventanas
}


