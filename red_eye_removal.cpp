
#include "stdafx.h"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"   


//#include "rgb.h"

#include <iostream>
#include <stdio.h>
#include "dirent.h"
#include <sstream>
#include <iomanip>
#include <string.h>
#include <math.h>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );

/** Global variables */
//-- Note, either copy these two files from opencv/data/haarscascades to your current folder, or change these locations
String face_cascade_name = "lbpcascade_frontalface.xml";
String eyes_cascade_name = "haarcascade_eye.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

/**
 * @function main
 */
int main( void )
{
 CvCapture* capture;
	Mat frame;

	if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
    if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
	capture = cvCaptureFromCAM( -1 );


//	if( false )
	if( capture )
	{
		while( true )
		{
			frame = cvQueryFrame( capture );
			if( !frame.empty() )
			{ detectAndDisplay( frame ); }
			else
			{ printf(" --(!) No captured frame -- Break!"); break; }

			int c = waitKey(10);
			if( (char)c == 'c' ) { break; }
		}
	} else {

		DIR *dir;
		struct dirent *ent;
		char inputFolder[] = "inputImages//";
		if ((dir = opendir (inputFolder)) != NULL) {
			while ((ent = readdir (dir)) != NULL) {
				std::ostringstream oss;
				oss << inputFolder << ent->d_name;
				Mat img = imread(oss.str());
				if( img.data )
				{
					//detectAndSave(img, ent->d_name);
					detectAndDisplay(img);
				}
			}
			closedir (dir);
			waitKey(0);
		} else {
			perror ("");
			return EXIT_FAILURE;
		}
	}

  return 0;
}



/**
 * @function detectAndDisplay
 */
void detectAndDisplay( Mat frame )
{
   std::vector<Rect> faces;
   Mat frame_gray;

   cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
   equalizeHist( frame_gray, frame_gray );
   vector<Mat> hsv;
   //-- Detect faces
   face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );


   imshow( "Original",frame );
   waitKey(100);
		  
   for( size_t i = 0; i < faces.size(); i++ )
    {
      Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
      //ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2), 0, 0, 360, Scalar( 255, 0, 255 ), 2, 8, 0 );

      Mat faceROI = frame( faces[i] );
      std::vector<Rect> eyes;

      //-- In each face, detect eyes
      eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(10, 10) );


      for( size_t j = 0; j < eyes.size(); j++ )
       {
		   Mat eyeROI ;
		  
		   cvtColor( frame, eyeROI, COLOR_RGB2HSV );
		   split(eyeROI,hsv);
		   
		  
		   


		   for(int m = faces[i].y + eyes[j].y + (eyes[j].height/4),b=0; m < faces[i].y + eyes[j].y + (eyes[j].height/4),b<eyes[j].height; m++,b++) {
			   for(int l = 3*(faces[i].x + eyes[j].x), c=0 ; l < 3*(faces[i].x + eyes[j].x + eyes[j].width),c<3*eyes[j].width; l++,c++) {
			   

				  // int w = frame.at<uchar>(m,l);

				   int hue=hsv[0].at<uchar>(m,l/3);
				   int saturation=hsv[1].at<uchar>(m,l/3);

				  // double hue1=hue/255.0;
				//   double saturation1=saturation/255.0;
				   

				   if (hue< 200 && saturation>140)
				   {
					frame.at<uchar>(m,l)= 0;
				   }
				
			 }
			 
		  }
		 Point eye_center( faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2 );
         int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
        // circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 3, 8, 0 );
       }
	   //imshow( window_name, faceROI );
         
    }
   //-- Show what you got
   imshow( window_name, frame );
}
