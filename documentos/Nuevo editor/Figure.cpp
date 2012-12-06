#include "Figure.hpp"
Figure::Figure(int r, int g, int b)
{
		color=CV_RGB(r,g,b);
		thickness=4;
		drawtype = '?';
		task = '?';
}

void Figure::showInfo()
{
	std::cout<<"First Point: "<<fp.x<<","<<fp.y<<"\n";
	std::cout<<"Second Point: "<<lp.x<<","<<lp.y<<"\n";
	std::cout<<"drawtype: "<<drawtype<<"\n";
	std::cout<<"thickness: "<<thickness<<"\n";
	std::cout<<"color1: "<<color.val[0]<<"\n";
	std::cout<<"color2: "<<color.val[1]<<"\n";
	std::cout<<"color3: "<<color.val[2]<<"\n";
	std::cout<<"color4: "<<color.val[3]<<"\n";
}

void Figure::draw(IplImage* image)
{	
	int ca, co, r;
	float cos60=0.5,sin60=0.8660; 
	float cos72=0.3090,sin72=0.9510;

	int xm=getHalfOf('x');
	int ym=getHalfOf('y');
	//r=(lp.x-fp.x)/2;
	r=lp.x-fp.x>=0?(lp.x-fp.x)/2:(fp.x-lp.x)/2;
	
	switch(drawtype)
	{
		case 1:		
		case 2://linea
			{
				cvLine(image,fp,lp,color,thickness,8);			
				break;
			}
		case 3://cuadrilatero
			{
				cvRectangle(image,fp,lp,color,thickness,8);
				break;
			}
		case 4://triangulo
			{				
				cvLine(image,cvPoint(fp.x<lp.x?fp.x:lp.x,fp.y<lp.y?lp.y:fp.y),cvPoint(lp.x>fp.x?lp.x:fp.x,lp.y>fp.y?lp.y:fp.y),color,thickness,8);	//base
				cvLine(image,cvPoint(xm,fp.y<lp.y?fp.y:lp.y),cvPoint(fp.x<lp.x?fp.x:lp.x,fp.y<lp.y?lp.y:fp.y),color,thickness,8); //lado izq
				cvLine(image,cvPoint(xm,fp.y<lp.y?fp.y:lp.y),cvPoint(lp.x>fp.x?lp.x:fp.x,lp.y>fp.y?lp.y:fp.y),color,thickness,8); //lado der
				break;
			}
		case 'c'://circulo
			{
				cvCircle(image,cvPoint(xm,ym),r,color,thickness,8);
				break;
			}
		case 5://elipse
			{
				int firstaxis, secondaxis;
				//firstaxis=(lp.x-fp.x)/2;
				firstaxis=lp.x-fp.x>=0?(lp.x-fp.x)/2:(fp.x-lp.x)/2;
				//secondaxis=(lp.y-fp.y)/2;	
				secondaxis=lp.y-fp.y>=0?(lp.y-fp.y)/2:(fp.y-lp.y)/2;	
				cvEllipse(image,cvPoint(xm,ym),cvSize(firstaxis,secondaxis),0,180,-180,color,thickness);
				break;
			}
		case 6://pentagono
			{
				ca=r*cos72;
				co=r*sin72;
				//CvPoint pp1= cvPoint(xm,fp.y);
				CvPoint pp1= cvPoint(xm,fp.y<lp.y?fp.y:lp.y);
				CvPoint pp2= cvPoint(xm+co,ym-ca);
				CvPoint pp3= cvPoint(xm-co,ym-ca);
				CvPoint pp4= cvPoint((xm+co)-ca,(ym-ca)+co);
				CvPoint pp5= cvPoint((xm-co)+ca,(ym-ca)+co);
				cvLine(image,pp1,pp2,color,thickness,8);//linea sup-der
				cvLine(image,pp2,pp4,color,thickness,8);//linea inf-der
				cvLine(image,pp4,pp5,color,thickness,8);//linea inf
				cvLine(image,pp5,pp3,color,thickness,8);//linea inf-izq
				cvLine(image,pp3,pp1,color,thickness,8);//linea sup-izqr			
				break;			
			}			
		case 7://Hexagono
			{								
				ca=r*cos60;
				co=r*sin60;
				//CvPoint ph1= cvPoint(lp.x,ym);
				CvPoint ph1= cvPoint(lp.x>fp.x?lp.x:fp.x,ym);
				CvPoint ph2= cvPoint(xm+ca,ym-co);
				CvPoint ph3= cvPoint(xm+ca,ym+co);
				//CvPoint ph4= cvPoint(fp.x,ym);
				CvPoint ph4= cvPoint(fp.x<lp.x?fp.x:lp.x,ym);
				CvPoint ph5= cvPoint(xm-ca,ym-co);
				CvPoint ph6= cvPoint(xm-ca,ym+co);
				cvLine(image,ph1,ph2,color,thickness,8);//linea sup-der
				cvLine(image,ph2,ph5,color,thickness,8);//linea sup
				cvLine(image,ph5,ph4,color,thickness,8);//linea sup-izq
				cvLine(image,ph4,ph6,color,thickness,8);//linea inf-izq
				cvLine(image,ph6,ph3,color,thickness,8);//linea inf
				cvLine(image,ph3,ph1,color,thickness,8);//linea inf-der
				break;			
			}
		default: std::cout<<"\n!!Aviso no hay figura seleccionada¡¡\n";drawtype = '?';break;
	}
}

void Figure::changePoints()
{
		int x,y;
		if(fp.x < lp.x)
		{
				if(fp.y > lp.y)
				{
						y = fp.y;
						fp.y = lp.y;
						lp.y = y;
				}
		}
		else
		{
			if(fp.y > lp.y)
			{
					x = fp.x;
					y = fp.y;
					fp.x = lp.x;
					fp.y = lp.y;
					lp.y = y;
					lp.x = x;
			}
			else
			{
					x = fp.x;
					fp.x = lp.x;
					lp.x = x;
			}
		}
}

bool Figure::selected(int x, int y)
{
	if(drawtype != 'f' && drawtype != 'l')
		changePoints();
	//l y r son lados de la ecuación de la recta (x2-x1)(y-y1)=(y2-y1)(x-x1)
	// l es el lado Izq de la ecuación anterior y r es el lado Der de la ecuación
	int l, r;
	
	// l(1-6) son el numero de lados de un poligono
	// si l(1-6) es igual a false, indica si el punto(x,y) se encuentra fuera de la figura
	// de lo contrario l(1-6) es igual a true
	bool l1, l2, l3, l4, l5, l6;
	l1 = l2 = l3 = l4 = l5 = l6 = false;
	
	//variables para calcular los puntos del pentagono y hexagono
	float cos60=0.5,sin60=0.8660; 
	float cos72=0.3090,sin72=0.9510;
	int ca, co, R;
	R=(lp.x-fp.x)/2;
	
	int xm=getHalfOf('x');
	int ym=getHalfOf('y');
	switch(drawtype)
	{
			case 2:
			{
				l = getPartOfEquLine(lp.x, fp.x, y, fp.y);
				r = getPartOfEquLine(lp.y, fp.y, x, fp.x);
				if((l-r)>=-100 && (l-r)<=100) //verifica si el click esta sobre la recta
					return true;
				break;
			}
			case 4:
			{
				l = getPartOfEquLine(fp.x, xm, y, fp.y);
				r = getPartOfEquLine(lp.y, fp.y, x, xm);
				if(l <= r)
					l1 = true; //verifica si el click esta a la derecha del lado Izq del triangulo
				l = getPartOfEquLine(lp.x, fp.x, y, lp.y);
				r = getPartOfEquLine(lp.y, lp.y, x, fp.x);
				if(l <= r)
					l2 = true; //verifica si el click esta arriba del lado base del triangulo
				l = getPartOfEquLine(lp.x, xm, y, fp.y);
				r = getPartOfEquLine(lp.y, fp.y, x, xm);
				if(l >= r)
					l3 = true; //verifica si el click esta a la izquierda del lado Der del triangulo
				if(l1 && l2 && l3) // si  cumple con las 3 condiciones anteriores el click esta dentro del triangulo 
					return true;
				break;
			}
			case 3:
			{
				if(x>=fp.x && x<=lp.x && y>=fp.y && y<=lp.y)// verifica si el punto se encuentra dentro del cuadrado
					return true;
				break;
			}
			case 'c':
			{
				if(getPartOfEquCircle(x, xm, y, ym)<=pow(xm,2))
					return true;
				break;
			}
			case 5:
			{
				if(getEquEllipse(x, xm, y, ym)<=1)
					return true;
				break;
			}
			case 6:
			{
				ca=R*cos72;
				co=R*sin72;
				CvPoint pp1= cvPoint(xm,fp.y);
				CvPoint pp2= cvPoint(xm+co,ym-ca);
				CvPoint pp3= cvPoint(xm-co,ym-ca);
				CvPoint pp4= cvPoint((xm+co)-ca,(ym-ca)+co);
				CvPoint pp5= cvPoint((xm-co)+ca,(ym-ca)+co);
				
				l = getPartOfEquLine(pp3.x, pp1.x, y, pp1.y);
				r = getPartOfEquLine(pp3.y, pp1.y, x, pp1.x);
				if(l <= r)
					l1 = true; // sup-izq
				l = getPartOfEquLine(pp2.x, pp1.x, y, pp1.y);
				r = getPartOfEquLine(pp2.y, pp1.y, x, pp1.x);
				if(l >= r)
					l2 = true; //sup-der
				l = getPartOfEquLine(pp4.x, pp2.x, y, pp2.y);
				r = getPartOfEquLine(pp4.y, pp2.y, x, pp2.x);
				if(l >= r)
					l3 = true; //inf-der
				l = getPartOfEquLine(pp5.x, pp4.x, y, pp4.y);
				r = getPartOfEquLine(pp5.y, pp4.y, x, pp4.x);
				if(l >= r)
					l4 = true; //inf
				l = getPartOfEquLine(pp3.x, pp5.x, y, pp5.y);
				r = getPartOfEquLine(pp3.y, pp5.y, x, pp5.x);
				if(l >= r)
					l5 = true; //inf-izq
				if(l1 && l2 && l3 && l4 && l5)
					return true;
				break;
			}
			case 7:
				ca=R*cos60;
				co=R*sin60;
				CvPoint ph1= cvPoint(lp.x,ym);
				CvPoint ph2= cvPoint(xm+ca,ym-co);
				CvPoint ph3= cvPoint(xm+ca,ym+co);
				CvPoint ph4= cvPoint(fp.x,ym);
				CvPoint ph5= cvPoint(xm-ca,ym-co);
				CvPoint ph6= cvPoint(xm-ca,ym+co);
				
				l = getPartOfEquLine(ph4.x, ph5.x, y, ph5.y);
				r = getPartOfEquLine(ph4.y, ph5.y, x, ph5.x);
				if(l <= r)
					l1 = true; // sup-izq
				l = getPartOfEquLine(ph5.x, ph2.x, y, ph2.y);
				r = getPartOfEquLine(ph5.y, ph2.y, x, ph2.x);
				if(l <= r)
					l2 = true; //sup
				l = getPartOfEquLine(ph1.x, ph2.x, y, ph2.y);
				r = getPartOfEquLine(ph1.y, ph2.y, x, ph2.x);
				if(l >= r)
					l3 = true; //sup-der
				l = getPartOfEquLine(ph3.x, ph1.x, y, ph1.y);
				r = getPartOfEquLine(ph3.y, ph1.y, x, ph1.x);
				if(l >= r)
					l4 = true; //inf-der
				l = getPartOfEquLine(ph6.x, ph3.x, y, ph3.y);
				r = getPartOfEquLine(ph6.y, ph3.y, x, ph3.x);
				if(l >= r)
					l5 = true; //inf
				l = getPartOfEquLine(ph4.x, ph6.x, y, ph6.y);
				r = getPartOfEquLine(ph4.y, ph6.y, x, ph6.x);
				if(l >= r)
					l6 = true; //inf-izq
				if(l1 && l2 && l3 && l4 && l5 && l6)
					return true;
				break;
	}
	return false;
}

int Figure::getPartOfEquLine(int val1, int val2, int val3, int val4)
{
	return (val1-val2)*(val3-val4);
}

int Figure::getPartOfEquCircle(int x, int h, int y, int k)
{
		return pow((x-h),2) + pow((y-k),2);
}

int Figure::getEquEllipse(int x, int h, int y, int k)
{
	int a, b;
	if((lp.x-fp.x)/2>(lp.y-fp.y)/2)
	{
		a=(lp.x-fp.x)/2;
		b=(lp.y-fp.y)/2;
		return (pow((x-h),2)/pow(a,2)) + (pow((y-k),2)/pow(b,2));
	}
	else
	{ 
		b=(lp.x-fp.x)/2;
		a=(lp.y-fp.y)/2;
		return (pow((x-h),2)/pow(b,2)) + (pow((y-k),2)/pow(a,2));
	}
	
}
int Figure::getHalfOf(char val)
{
		if(val == 'x')
			return (lp.x+fp.x)/2;
		else
			return (lp.y+fp.y)/2;
}

void Figure::setColor(int r, int g, int b)
{
		color = CV_RGB(r,g,b);
}

void Figure::setZoom(double p)
{
		int move;
		move = ((lp.x-fp.x)*(1-p))/2;
		fp.x = fp.x + move;
		lp.x = lp.x - move;
		move = ((lp.y-fp.y)*(1-p))/2;
		fp.y = fp.y + move;
		lp.y = lp.y - move;
}

void Figure::change()
{
	
}

void Figure::move(int dx, int dy)
{
		fp.x+=dx;
		fp.y+=dy;
		lp.x+=dx;
		lp.y+=dy;
}
