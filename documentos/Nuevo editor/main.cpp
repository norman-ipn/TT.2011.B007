#include"Window.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
Window window= Window(480, 300);
Figure figure=Figure(0,0,0);
std::list<Figure> list;
int xx = 0;
int yy = 0;
int lastState = 1;
int currentState = 1;
int shmid; 
int *variable;
key_t llave;


void drawList(IplImage* image, std::list<Figure> &list)
{
	std::list<Figure>::iterator pos;
	Figure temp = Figure(0,0,0);
	pos = list.begin();
	while(pos != list.end())
	{
			temp = *pos;
			temp.draw(image);
			pos++;
	}
}

Figure find(int x, int y, std::list<Figure> &list)
{
		Figure temp = Figure(0,0,0);
		std::list<Figure>::iterator pos;
		pos = list.begin();
		while(pos != list.end())
		{
				temp = *pos;
				if(temp.selected(x,y))
				{
					list.erase(pos);
					return temp;
				}
				pos++;
		}
		temp.drawtype = '?';
		return temp;
}

void on_mouse(int event, int x, int y, int flags, void* param)
{
	
	
	switch(event)
	{
		case CV_EVENT_LBUTTONUP://evento del boton cuando se suelta
			window.followmousemove = 0;
			if(figure.task != '9')
			{
				figure.lp.x=x;
				figure.lp.y=y;
			}	
			else
			{
				window.refresh();
				figure.task = '?';
			}
			if(figure.drawtype != '?' )
				list.push_back(figure);
			drawList(window.image, list);
							
			break;
		case CV_EVENT_LBUTTONDOWN://evento del boton cuando se presiona
			xx = x;
			yy = y;
			
			window.followmousemove = 1;
			
			if(variable[3] != 9 && variable[3]<50)
			{
				figure = Figure(variable[0],variable[1],variable[2]);
				figure.fp.x=x;
				figure.fp.y=y;
				figure.drawtype=variable[3];
			}
			else
			{
				figure = find(x,y,list);
				if(variable[4]==50 || variable[4]==75 || variable[4]==125 || variable[4]==150)
				{
					figure.setZoom((float)variable[4]/100);
					figure.task = '9';
				}
				if(variable[0]!=-1 && variable[1]!=-1 && variable[2]!=-1)
				{
					figure.setColor(variable[0],variable[1],variable[2]);
					figure.task = '9';
				}

				if(variable[3]==9)
					figure.task = '9';
			}
			break;
	}
	if(CV_EVENT_MOUSEMOVE==event && window.followmousemove == 1)//evento del boton en movimiento
	{	

		window.refresh();
		if(figure.task == '9')
		{
			figure.move(x-xx,y-yy);
			xx=x;
			yy=y;
		}
		else
		{
			figure.lp.x = x;
			figure.lp.y = y;
		}
		
		figure.draw(window.image);
		drawList(window.image, list);	
	}
}
void SignalHandler (int signal)
{
	int event;
	switch(signal)
	{
		case 10:
			printf("mouse click \n contenido de la memoria\tx1: %d\tx2: %d\n",variable[4],variable[5]);
			currentState = 0;
		break;
		case 30:
			printf("mouse release \n contenido de la memoria\tx1: %d\tx2: %d\n",variable[4],variable[5]);
			currentState = 1;
		break;
	}
	if(lastState == 0 && currentState == 0)
		event = 0;
	if(lastState == 1 && currentState == 0)
		event = 1;
	if(lastState == 0 && currentState == 1)
		event = 4;
	if(!(lastState == 1 && currentState == 1))
	{
		on_mouse(event, variable[6],variable[7], 1, NULL);
	}
		
	lastState = currentState;
	  
}

int main(int argc, char* argv[])
{

	llave = ftok("kinectTool", 'k');	
	if((shmid = shmget(llave,  sizeof(int)*8, IPC_CREAT | 0600)) == -1)
	{
		perror("Error en shmget");
		exit(-1);
	}
	
	if((variable = (int *)shmat(shmid, NULL, 0)) == (int *)(-1))
	{
		perror("Fallo shmat");
		exit(-1);
	}
	variable[5] = getpid();
	cvSetMouseCallback("2011-B007", on_mouse);	
	signal (10, SignalHandler);
	signal (30, SignalHandler);
	window.show();
	
	shmctl(shmid, IPC_RMID, 0);
	return 0;
}
