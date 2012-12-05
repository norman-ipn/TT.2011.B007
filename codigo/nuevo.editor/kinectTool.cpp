#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char* argv[])
{
  int shmid; 
		int *variable;
		key_t llave;
  
  CvMoments moments;
  CvHuMoments humoments;
  CvSeq* contour;
  CvSeq* contourLow;
  CvSeq* supportContour;
  
    std::cout << "Kinect opening ..." << std::endl;
    cv::VideoCapture capture( CV_CAP_OPENNI );
    
    if( !capture.isOpened() )
    {
        std::cout << "Can not open a capture object." << std::endl;
	return -1;
    }
    else
    {
	std::cout << "done." << std::endl;
    }
    
    capture.set( CV_CAP_OPENNI_IMAGE_GENERATOR_OUTPUT_MODE, CV_CAP_OPENNI_VGA_30HZ );
    
    cvNamedWindow("Main", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Support", CV_WINDOW_AUTOSIZE);
    
    //memoria compartida
  
    llave = ftok("kinectTool",'k');
  
    if((shmid = shmget(llave, sizeof(int) * 8, IPC_CREAT | 0600)) == -1){
      perror("Error en el shmget");
      exit(-1);
    }
  
  
    if((variable = (int *)shmat(shmid,NULL,0)) == (int *)(-1)){
      perror("Fallo shmat");
      exit(-1);
    }
	
	
	
	while(1)
	{ 
	  cv::Mat bgrImage;
	  CvScalar avg = cvScalar( 0.0, 0.0, 0.0, 0.0 );
	  CvScalar stdDev = cvScalar( 0.0, 0.0, 0.0, 0.0 );
	  contour = 0;
	  contourLow = 0;
	  supportContour = 0;
	  CvMemStorage* storage = cvCreateMemStorage(0);
	  CvRect choiceRect = cvRect( 0, 0, 0, 0 );
	  CvRect supportRect = cvRect( 0, 0, 0, 0 );
	  CvPoint initPoint = cvPoint( 0, 0 );
	  CvPoint endPoint = cvPoint( 0, 0 );
	  CvRect positionRect = cvRect( 0, 0, 0, 0);
	  CvPoint positionPoint = cvPoint( 0, 0);
	  CvPoint auxiliar = cvPoint( 0, 0);
	  
	  CvPoint tagArea1 = cvPoint( 0, 150);
	  CvPoint tagArea2 = cvPoint( 0, 220);
	  CvPoint tagArea3 = cvPoint( 0, 290);
	  CvPoint tagArea4 = cvPoint( 0, 360);
	  
	  int cuadrante = 0;
	  int tagPosition = 0;
	  
	  
	  if( !capture.grab() )
	  {
	      std::cout << "Can not grab images." << std::endl;
	  }
	    
	  capture.retrieve( bgrImage, CV_CAP_OPENNI_BGR_IMAGE );
	  IplImage* mainImage = new IplImage( bgrImage);
	  for(int j=0;j<mainImage->height;j++)
	  {
	    for(int i=0;i<mainImage->width;i++)
	    {
		CvScalar pixel = cvGet2D(mainImage, j, i);
		if(pixel.val[2]<=5 && pixel.val[1]<=5 && pixel.val[0]<=5)
		{
		  pixel.val[0]=pixel.val[1]=pixel.val[2]=0;
		}
		else
		{
		 pixel.val[0]=pixel.val[1]=pixel.val[2]=255;
		}
		  cvSet2D(mainImage, j, i, pixel);
		}
	  }
	  IplImage* supportImage = cvCreateImage(cvSize(mainImage->width,mainImage->height), IPL_DEPTH_8U,1);
	  cvCvtColor( mainImage, supportImage, CV_RGB2GRAY );
	  cvSmooth(supportImage, supportImage, CV_GAUSSIAN);
	  cvEqualizeHist(supportImage,supportImage);
	  cvAvgSdv(supportImage, &avg, &stdDev, NULL);
	  cvThreshold(supportImage, supportImage, (int)avg.val[0]-7*(int)(stdDev.val[0]/8),255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
	  cvErode(supportImage,supportImage,NULL,1);
	  cvDilate(supportImage,supportImage,NULL,1);cvShowImage("Support", supportImage);
	  
	  cvFindContours(supportImage, storage, &contour, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
	  contourLow = cvApproxPoly(contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP,1,1);
		
	  cvRectangle(mainImage, cvPoint(0,0), cvPoint(150,mainImage->height), CV_RGB(0,255,255), 3, 8, 0);
	  cvLine(mainImage, tagArea1, cvPoint(150,tagArea1.y), CV_RGB(0,255,255), 3, 8, 0);
	  cvLine(mainImage, tagArea2, cvPoint(150,tagArea2.y), CV_RGB(0,255,255), 3, 8, 0);
	  cvLine(mainImage, tagArea3, cvPoint(150,tagArea3.y), CV_RGB(0,255,255), 3, 8, 0);
	  cvLine(mainImage, tagArea4, cvPoint(150,tagArea4.y), CV_RGB(0,255,255), 3, 8, 0);
		
	  for(;contourLow != 0; contourLow = contourLow->h_next)
	  {
	    cvMoments(contourLow, &moments, 0);
	    cvGetHuMoments(&moments, &humoments);
	    
	    supportContour = contourLow;
	    
	    if(moments.m00 >= 800 && moments.m00 <= 60000 && humoments.hu1 >= 0.157 && humoments.hu1 <= 0.167)
	    {
		/*cvDrawContours(mainImage,contourLow, CV_RGB(255,0,0), CV_RGB(0,255,0),-1,3,8,cvPoint(0,0));
	
		if(contourLow->v_next != NULL)
		    contourLow = contourLow->v_next;
		else
		    break;
		
		if(contourLow->v_next != NULL)
		    contourLow = contourLow->v_next;
		else
		    break;
		*/
		cvDrawContours(mainImage,contourLow, CV_RGB(0,0,255), CV_RGB(0,255,0),-1,3,8,cvPoint(0,0));
		
		choiceRect = cvBoundingRect(contourLow, 1);
		
		initPoint.x = choiceRect.x;
		initPoint.y = choiceRect.y;
		endPoint.x = (choiceRect.x+choiceRect.width);
		endPoint.y = (choiceRect.y+choiceRect.height);
		supportRect.width = (int)(choiceRect.width/3);
		supportRect.height = (int)(choiceRect.height/3);
		supportRect.x = (choiceRect.x + supportRect.width) + 3;
		supportRect.y = (choiceRect.y + supportRect.height) + 3;
		supportRect.width = supportRect.width - 6;
		supportRect.height = supportRect.height - 6;
		
		
		cvRectangle(mainImage, initPoint, endPoint, CV_RGB(255,255,0), 1, 8, 0);
		cvLine(mainImage, cvPoint(initPoint.x, supportRect.y), cvPoint(endPoint.x,supportRect.y), CV_RGB(255,255,0), 1, 8, 0);
		cvLine(mainImage, cvPoint(initPoint.x, supportRect.y + supportRect.height), cvPoint(endPoint.x,supportRect.y + supportRect.height), CV_RGB(255,255,0), 1, 8, 0);
		cvLine(mainImage, cvPoint(supportRect.x, initPoint.y), cvPoint(supportRect.x,endPoint.y), CV_RGB(255,255,0), 1, 8, 0);
		cvLine(mainImage, cvPoint(supportRect.x + supportRect.width, initPoint.y), cvPoint(supportRect.x  + supportRect.width, endPoint.y), CV_RGB(255,255,0), 1, 8, 0);
		
		
		if(contourLow->v_next != NULL)
		    contourLow = contourLow->v_next;
		else
		    break;
		
		positionRect = cvBoundingRect(contourLow, 1);
		positionPoint.x = positionRect.x + (int)(positionRect.width/2);
		positionPoint.y = positionRect.y + (int)(positionRect.height/2);
		cvLine(mainImage, positionPoint, positionPoint, CV_RGB(255,0,0), 3, 8, 0);
		
		if(positionPoint.x > supportRect.x && positionPoint.x < (supportRect.x + supportRect.width) && positionPoint.y < supportRect.y)
		{
		  cuadrante = 0;
		}  
		else if(positionPoint.x >= (supportRect.x + supportRect.width) && positionPoint.y <= supportRect.y)
		{
		  cuadrante = 1;
		}
		else if(positionPoint.x > (supportRect.x + supportRect.width) && positionPoint.y > supportRect.y && positionPoint.y < (supportRect.y + supportRect.height))
		{
		  cuadrante = 2;
		}
		else if(positionPoint.x >= (supportRect.x + supportRect.width) && positionPoint.y >= (supportRect.y + supportRect.height)){
		  cuadrante = 3;
		}
		else if(positionPoint.x > supportRect.x && positionPoint.x < (supportRect.x + supportRect.width) && positionPoint.y > (supportRect.y + supportRect.height))
		{
		  cuadrante = 4;
		}
		else if(positionPoint.x <= supportRect.x && positionPoint.y >= (supportRect.y + supportRect.height))
		{
		  cuadrante = 5;
		} 
		else if(positionPoint.x < supportRect.x && positionPoint.y > supportRect.y && positionPoint.y < (supportRect.y + supportRect.height))
		{
		  cuadrante = 6;
		}  
		else if(positionPoint.x <= supportRect.x && positionPoint.y <= supportRect.y)
		{
		  cuadrante = 7;
		}
		
		if(positionPoint.y <= tagArea1.y)
		{
			tagPosition = 0;
		}
		else if(positionPoint.y > tagArea1.y && positionPoint.y <= tagArea2.y)
		{
			tagPosition = 1;
		}
		else if(positionPoint.y > tagArea2.y && positionPoint.y <= tagArea3.y)
		{
			tagPosition = 2;
		}
		else if(positionPoint.y > tagArea3.y && positionPoint.y <= tagArea4.y)
		{
			tagPosition = 3;
		}
		else if(positionPoint.y > tagArea4.y && positionPoint.y <= mainImage -> height)
		{
			tagPosition = 4;
		}

		std::cout << "Tag No: "<< tagPosition << "\t Cuadrante No: " << cuadrante << std::endl;
		
		switch(tagPosition){
		  
		  case 0:
		   
		  case 1:
		  
		  case 2:
			if( cuadrante == 0 )
		      variable[tagPosition] = 0;
		    else if( cuadrante == 6)
		      variable[tagPosition] = 85;
		    else if( cuadrante == 4)
		      variable[tagPosition] = 170;
		    else if( cuadrante == 2)
		      variable[tagPosition] = 255;
		    else
		      variable[tagPosition] = -1;
		    break;
			
		  case 3:
		    if( cuadrante == 0 )
		      variable[tagPosition] = 1;
		    else if( cuadrante == 7)
		      variable[tagPosition] = 2;
		    else if( cuadrante == 6)
		      variable[tagPosition] = 3;
		    else if( cuadrante == 5)
		      variable[tagPosition] = 4;
		    else if( cuadrante == 4)
		      variable[tagPosition] = 5;
		    else if( cuadrante == 3)
		      variable[tagPosition] = 6;
		    else if( cuadrante == 2)
		      variable[tagPosition] = 7;
		    else 
		      variable[tagPosition] = 9;
		    break;
		  
		  case 4:
		    if( cuadrante == 0 )
		      variable[tagPosition] = 50;
		    else if( cuadrante == 6)
		      variable[tagPosition] = 75;
		    else if( cuadrante == 4)
		      variable[tagPosition] = 125;
		    else if( cuadrante == 2)
		      variable[tagPosition] = 150;
		    else
		      variable[tagPosition] = -1;
		    break;
		}
		printf("%d,%d,%d,%d,%d", variable[0],variable[1],variable[2],variable[3], variable[4]);
		contourLow = supportContour;
		
	    }
	  }
	  
	  
	  cvShowImage("Main", mainImage);
	     
	  delete mainImage;
	  bgrImage.release();
	  
	  if((cvWaitKey(100)&255)==27)break;
	}
	shmctl (shmid, IPC_RMID, (struct shmid_ds *)NULL);
	cvDestroyAllWindows();
	return 0;
}
