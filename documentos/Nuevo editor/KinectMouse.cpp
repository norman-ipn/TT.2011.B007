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

using namespace cv;
using namespace std;



void colorizeDisparity( const Mat& gray, Mat& rgb, double maxDisp=-1.f, float S=1.f, float V=1.f )
{
    CV_Assert( !gray.empty() );
    CV_Assert( gray.type() == CV_8UC1 );

    if( maxDisp <= 0 )
    {
        maxDisp = 0;
        minMaxLoc( gray, 0, &maxDisp );
    }

    rgb.create( gray.size(), CV_8UC3 );
    rgb = Scalar::all(0);
    if( maxDisp < 1 )
        return;

    for( int y = 0; y < gray.rows; y++ )
    {
        for( int x = 0; x < gray.cols; x++ )
        {
            uchar d = gray.at<uchar>(y,x);
            unsigned int H = ((uchar)maxDisp - d) * 240 / (uchar)maxDisp;

            unsigned int hi = (H/60) % 6;
            float f = H/60.f - H/60;
            float p = V * (1 - S);
            float q = V * (1 - f * S);
            float t = V * (1 - (1 - f) * S);

            Point3f res;
            
            if( hi == 0 ) //R = V,	G = t,	B = p
                res = Point3f( p, t, V );
            if( hi == 1 ) // R = q,	G = V,	B = p
                res = Point3f( p, V, q );
            if( hi == 2 ) // R = p,	G = V,	B = t
                res = Point3f( t, V, p );
            if( hi == 3 ) // R = p,	G = q,	B = V
                res = Point3f( V, q, p );
            if( hi == 4 ) // R = t,	G = p,	B = V
                res = Point3f( V, p, t );
            if( hi == 5 ) // R = V,	G = p,	B = q
                res = Point3f( q, p, V );

            uchar b = (uchar)(std::max(0.f, std::min (res.x, 1.f)) * 255.f);
            uchar g = (uchar)(std::max(0.f, std::min (res.y, 1.f)) * 255.f);
            uchar r = (uchar)(std::max(0.f, std::min (res.z, 1.f)) * 255.f);

            rgb.at<Point3_<uchar> >(y,x) = Point3_<uchar>(b, g, r);     
        }
    }
}

float getMaxDisparity( VideoCapture& capture )
{
    const int minDistance = 400; // mm
    float b = (float)capture.get( CV_CAP_OPENNI_DEPTH_GENERATOR_BASELINE ); // mm
    float F = (float)capture.get( CV_CAP_OPENNI_DEPTH_GENERATOR_FOCAL_LENGTH ); // pixels
    return b * F / minDistance;
}

void parseCommandLine( int argc, char* argv[], bool& isColorizeDisp, bool& isFixedMaxDisp, bool& isSetSXGA, bool retrievedImageFlags[] )
{
    // set defaut values
    isColorizeDisp = true;
    isFixedMaxDisp = false;
    isSetSXGA = false;

    retrievedImageFlags[0] = false;
    retrievedImageFlags[1] = true;
    retrievedImageFlags[2] = false;
    retrievedImageFlags[3] = true;
    retrievedImageFlags[4] = false;

    if( argc == 1 )
    {
        
    }
    else
    {
        for( int i = 1; i < argc; i++ )
        {
            if( !strcmp( argv[i], "--help" ) || !strcmp( argv[i], "-h" ) )
            {
               
                exit(0);
            }
            else if( !strcmp( argv[i], "-cd" ) )
            {
                isColorizeDisp = atoi(argv[++i]) == 0 ? false : true;
            }
            else if( !strcmp( argv[i], "-fmd" ) )
            {
                isFixedMaxDisp = atoi(argv[++i]) == 0 ? false : true;
            }
            else if( !strcmp( argv[i], "-sxga" ) )
            {
                isSetSXGA = atoi(argv[++i]) == 0 ? false : true;
            }
            else if( !strcmp( argv[i], "-m" ) )
            {
                string mask( argv[++i] );
                if( mask.size() != 5)
                    CV_Error( CV_StsBadArg, "Incorrect length of -m argument string" );
                int val = atoi(mask.c_str());

                int l = 100000, r = 10000, sum = 0;
                for( int i = 0; i < 5; i++ )
                {
                    retrievedImageFlags[i] = ((val % l) / r ) == 0 ? false : true;
                    l /= 10; r /= 10;
                    if( retrievedImageFlags[i] ) sum++;
                }

                if( sum == 0 )
                {
                    cout << "No one output image is selected." << endl;
                    exit(0);
                }
            }
            else
            {
                cout << "Unsupported command line argument: " << argv[i] << "." << endl;
                exit(-1);
            }
        }
    }
}

/*
 * To work with Kinect the user must install OpenNI library and PrimeSensorModule for OpenNI and
 * configure OpenCV with WITH_OPENNI flag is ON (using CMake).
 */
int main( int argc, char* argv[] )
{
    int shmid; 
	int *variable;
	int idproceso;
	key_t llave;
	
	llave = ftok("finger", 'k');
	
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
    idproceso = variable[5];
    
    bool isColorizeDisp, isFixedMaxDisp, isSetSXGA;
    bool retrievedImageFlags[5];
    parseCommandLine( argc, argv, isColorizeDisp, isFixedMaxDisp, isSetSXGA, retrievedImageFlags );

    cout << "Kinect opening ..." << endl;
    VideoCapture capture( CV_CAP_OPENNI );
    cout << "done." << endl;

    if( !capture.isOpened() )
    {
        cout << "Can not open a capture object." << endl;
        return -1;
    }

    if( isSetSXGA )
        capture.set( CV_CAP_OPENNI_IMAGE_GENERATOR_OUTPUT_MODE, CV_CAP_OPENNI_SXGA_15HZ );
    else
        capture.set( CV_CAP_OPENNI_IMAGE_GENERATOR_OUTPUT_MODE, CV_CAP_OPENNI_VGA_30HZ ); // default
	cvNamedWindow("KinectMouse", CV_WINDOW_AUTOSIZE);
	CvPoint last;
    for(;;)
    {
        
       
        Mat disparityMap;
        Mat bgrImage;
       

        if( !capture.grab() )
        {
            cout << "Can not grab images." << endl;
            //return -1;
        }
        else
        {

            if( retrievedImageFlags[1] && capture.retrieve( disparityMap, CV_CAP_OPENNI_DISPARITY_MAP ) )
            {
                if( isColorizeDisp )
                {
                    Mat colorDisparityMap;
                    colorizeDisparity( disparityMap, colorDisparityMap, isFixedMaxDisp ? getMaxDisparity(capture) : -1 );
                    Mat validColorDisparityMap;
                    colorDisparityMap.copyTo( validColorDisparityMap, disparityMap != 0 );
                    imshow( "colorized disparity map", validColorDisparityMap );
                    IplImage* holeImage = new IplImage(validColorDisparityMap);
                    int minG = 90;
                    CvScalar pixel;
                    //Cambia los pixeles a blanco y negro, si el pixel 255 en rojo y minG en verde lo coloca blanco de lo contrario negro
					for(int j=0; j<holeImage->height;j++)
					{
						for(int i=0;i<holeImage->width;i++)
						{
							pixel = cvGet2D(holeImage, j, i);
							if(pixel.val[2]==255 && pixel.val[1] >=80 && pixel.val[1] <=100)
							{
								pixel.val[0]=255;
								pixel.val[1]=0;
								pixel.val[2]=0;
							}
							else
							{	
								pixel.val[0]=0;
								pixel.val[1]=0;
								pixel.val[2]=0;
							}
							cvSet2D(holeImage, j, i, pixel);
						}
					}
					IplImage* imageFinger;
					imageFinger = cvCreateImage(cvSize(holeImage->width,holeImage->height), IPL_DEPTH_8U,1);
						
					cvCvtColor( holeImage, imageFinger, CV_RGB2GRAY );
					cvFlip(imageFinger,imageFinger,1);
					cvSmooth(imageFinger, imageFinger, CV_GAUSSIAN);
					cvEqualizeHist(imageFinger,imageFinger);//this new line
					CvScalar avg;
					CvScalar avgStd;
					
					cvAvgSdv(imageFinger, &avg, &avgStd, NULL);
					cvThreshold(imageFinger, imageFinger, (int)avg.val[0]-7*(int)(avgStd.val[0]/8),255, CV_THRESH_OTSU); //otsu is the best option
					
					//contourImage = cvCloneImage(open_morf);
					CvMemStorage* storageFinger = cvCreateMemStorage(0);
					CvSeq* contourFinger = 0;
					CvSeq* contourFingerLow = 0;
					cvFindContours(imageFinger, storageFinger, &contourFinger, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
					contourFingerLow = cvApproxPoly(contourFinger, sizeof(CvContour), storageFinger, CV_POLY_APPROX_DP,1,1);
					CvMoments moments;
					CvHuMoments humoments;
					int areaMax = 1000;
					CvSeq* elem = 0;
					
					cvRectangle(holeImage,cvPoint(50,80),cvPoint(480,380),CV_RGB(255,255,255),3,8);
					CvRect rect;
					CvPoint pt1, pt2, ptr;
					for(;contourFingerLow != 0; contourFingerLow = contourFingerLow->h_next)
					{
						cvMoments(contourFingerLow, &moments, 0);
						cvGetHuMoments(&moments, &humoments);
						rect= cvBoundingRect(contourFingerLow, 1);
						
						pt1.x = rect.x;
						pt2.x = (rect.x+rect.width);
						pt1.y = rect.y;
						pt2.y = (rect.y+rect.height);
						
						ptr.x = rect.x+rect.width/2;
						ptr.y = rect.y+rect.height/2;
						if(humoments.hu1>0.1 && humoments.hu1<0.4 && ptr.x>50 && ptr.x <480 && ptr.y>80 && ptr.y<380)
						{
							elem = contourFingerLow;
						}
						
					}
					
					
					if(elem != 0)
					{
						rect= cvBoundingRect(elem, 1);
						ptr.x = rect.x+rect.width/2;
						ptr.y = rect.y+rect.height/2;
						last = ptr;
						cvCircle( holeImage, ptr, 5, CV_RGB(255,255,255), 4, 8,0);
						cvDrawContours(holeImage,elem, CV_RGB(255,0,0), CV_RGB(0,255,0),-1,3,8,cvPoint(0,0));
						printf("envia down\n");
						variable[6] = ptr.x-50;
						variable[7] = ptr.y-80;
						kill (idproceso, 10);
					}
					else
					{
						printf("envia up\n");
						variable[6] = last.x-50;
						variable[7] = last.y-80;
						kill (idproceso, 30);
					}
					cvShowImage("KinectMouse", holeImage);
				}
                else
                {
                    imshow( "original disparity map", disparityMap );
                }
            }
        }
		
        if( waitKey( 100 ) == 27 )
            break;
    }
	cvDestroyAllWindows();
    return 0;
}
