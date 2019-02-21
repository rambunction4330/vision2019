#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits>

using namespace cv;
using namespace std;

/// Global variables
Mat src, src_hls, res, color_filtered, test1, test2, test3;
int iLowH = 0;
int iHighH = 255;
int iLowS = 104; 
int iHighS = 255;
int iLowL = 57;
int iHighL = 255;
int thresh = 100;
int max_thresh = 255;
const char* controls_window = "Controls";
const char* color_filter_window = "Binary Image";
const char* cleaned_window = "Cleaned Image";
const double _targetWidth = 3.31;
const double _targetHeight = 5.83;

/// Function header
void clean_demo( int, void* );

/** @function main */
int main( int, char** argv )
{
  /// Load source image and convert it to gray
  src = imread( argv[1], 1 );
  resize(src, res, Size(), .6, .6);
  cvtColor( res, src_hls, CV_BGR2HSV );

  /// Create a window and a trackbar
  namedWindow( controls_window, CV_WINDOW_AUTOSIZE );
  createTrackbar( "LowH: ", controls_window, &iLowH, 255, clean_demo );
  createTrackbar( "HighH: ", controls_window, &iHighH, 255, clean_demo );
  createTrackbar( "LowS: ", controls_window, &iLowS, 255, clean_demo );
  createTrackbar( "HighS: ", controls_window, &iHighS, 255, clean_demo );
  createTrackbar( "LowL: ", controls_window, &iLowL, 255, clean_demo );
  createTrackbar( "HighL: ", controls_window, &iHighL, 255, clean_demo );
  createTrackbar( " Canny thresh:", controls_window, &thresh, max_thresh, clean_demo );
  imshow( controls_window, res );

  clean_demo( 0, 0 );

  waitKey(0);
  return(0);
}

void clean_demo( int, void* )
{
  printf( "(%d,%d,%d) to (%d,%d,%d)\n", iLowH, iLowS, iLowL, iHighH, iHighS, iHighL );

  inRange( src_hls, Scalar(iLowH, iLowS, iLowL), Scalar(iHighH, iHighS, iHighL), color_filtered ); 
  namedWindow( color_filter_window, CV_WINDOW_AUTOSIZE );
  imshow( color_filter_window, color_filtered );
  cvtColor( res , test1 , CV_BGR2GRAY);
  //blur( test1, test2, Size(3,3) );
  Canny(color_filtered , test2 , thresh , thresh*2 , 3);

  std::vector < std::vector<Point> > contours;
  std::vector < std::vector<Point> > filteredContours;
  std::vector<Point2d> centers;
  
  Mat tmpBinaryImage = color_filtered.clone();
  findContours(test2, contours, RETR_LIST, CHAIN_APPROX_NONE);
  Mat cleanedImage;
  cvtColor( test2, cleanedImage, CV_GRAY2RGB );
  

  cleanedImage.setTo(Scalar(255,255,255));
  
  for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++) {
    Point2d center;
    const Moments moms = moments(Mat(contours[contourIdx]));
	
    // filter blobs which are too small
    double area = moms.m00;
    if ( area < 300 ) {
      continue;
    }
    Rect rect = boundingRect(contours[contourIdx]);
    double width = rect.width;
    double height = rect.height;
    double aspectRatio = height / width;
    double perfectAspectRatio = _targetHeight/_targetWidth;
    cout << "Perfect A/S: " << perfectAspectRatio << endl;
    cout << "A/S: " << aspectRatio << endl;
    double aspectRatioTolerance = 0.55;
    if ( aspectRatio < perfectAspectRatio - aspectRatioTolerance ||
         aspectRatio > perfectAspectRatio + aspectRatioTolerance ) {
      continue;
    }
    cout << "Height:" << height << ", Width: " << width << endl;
    // else if(aspectRatio < smallAspectRatio - aspectRatioTolerance || aspectRatio > smallAspectRatio + aspectRatioTolerance){
	// continue;
	// }

       // else if(aspectRatio < bigAspectRatio - aspectRatioTolerance || aspectRatio > bigAspectRatio + aspectRatioTolerance){
	// continue;
//	}

    double rectangularness = area / ( width * height );
    if ( rectangularness < 0.5 ) {
      continue;
    }
	
    printf("Area is %.2f\n", area);
    center = Point2d(moms.m10 / moms.m00, moms.m01 / moms.m00);
    circle( cleanedImage, center, 2, Scalar(255,0,0), 2, 8, 0);
    filteredContours.push_back(contours[contourIdx]);
    centers.push_back(center);
    Canny(test1 , test2 , thresh , thresh*2 , 3);
  }

  drawContours( cleanedImage, filteredContours, -1, Scalar(0,255,0) );
  if ( centers.size() > 1 ) {
    double centerX = (centers[1].x + centers[2].x)/2;
    double centerY = (centers[1].y + centers[2].y)/2;
    printf("Aim Point is (%.2f, %.2f)\n", centerX, centerY);
    Point2d aimPoint = Point2d(centerX, centerY);
    circle(cleanedImage, aimPoint, 2, Scalar(0,0,255), 2, 8, 0);
    circle(cleanedImage, Point2d(centers[0].x, centers[0].y) , 2 , Scalar(255,0,255), 2 , 8 , 0);
  }
 
  namedWindow( cleaned_window, CV_WINDOW_AUTOSIZE );
  imshow( cleaned_window, cleanedImage );
}
