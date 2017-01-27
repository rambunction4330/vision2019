#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits>

using namespace cv;
using namespace std;

/// Global variables
Mat src, src_hsv, color_filtered;
int iLowH = 99;
int iHighH = 176;
int iLowS = 109; 
int iHighS = 255;
int iLowV = 85;
int iHighV = 255;

const char* controls_window = "Controls";
const char* color_filter_window = "Binary Image";
const char* cleaned_window = "Cleaned Image";

/// Function header
void clean_demo( int, void* );

/** @function main */
int main( int, char** argv )
{
  /// Load source image and convert it to gray
  src = imread( argv[1], 1 );
  cvtColor( src, src_hsv, CV_BGR2HSV );

  /// Create a window and a trackbar
  namedWindow( controls_window, CV_WINDOW_AUTOSIZE );
  createTrackbar( "LowH: ", controls_window, &iLowH, 179, clean_demo );
  createTrackbar( "HighH: ", controls_window, &iHighH, 179, clean_demo );
  createTrackbar( "LowS: ", controls_window, &iLowS, 255, clean_demo );
  createTrackbar( "HighS: ", controls_window, &iHighS, 255, clean_demo );
  createTrackbar( "LowV: ", controls_window, &iLowV, 255, clean_demo );
  createTrackbar( "HighV: ", controls_window, &iHighV, 255, clean_demo );
  imshow( controls_window, src );

  clean_demo( 0, 0 );

  waitKey(0);
  return(0);
}

void clean_demo( int, void* )
{
  printf( "(%d,%d,%d) to (%d,%d,%d)\n", iLowH, iLowS, iLowV, iHighH, iHighS, iHighV );

  inRange( src_hsv, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), color_filtered ); 
  namedWindow( color_filter_window, CV_WINDOW_AUTOSIZE );
  imshow( color_filter_window, color_filtered );

  std::vector < std::vector<Point> > contours;
  std::vector < std::vector<Point> > filteredContours;
  std::vector<Point2d> centers;
  std::vector<Point> shape;
  shape.push_back(Point2d(0,0));
  shape.push_back(Point2d(0,5));
  shape.push_back(Point2d(2,5));
  shape.push_back(Point2d(2,0));
  shape.push_back(Point2d(0,0));
  
  std::vector<Point> shape2;
  shape2.push_back(Point2d(0,0));
  shape2.push_back(Point2d(0,5));
  shape2.push_back(Point2d(10.25,5));
  shape2.push_back(Point2d(10.25,0));
  shape2.push_back(Point2d(0,0));
  Mat tmpBinaryImage = color_filtered.clone();
  findContours(tmpBinaryImage, contours, RETR_LIST, CHAIN_APPROX_NONE);
  Mat cleanedImage;
  cvtColor( color_filtered, cleanedImage, CV_GRAY2RGB );
  cleanedImage.setTo(Scalar(255,255,255));
  
  //double tempmatch = matchShapes(shape, shape, CV_CONTOURS_MATCH_I2, 0);
  //cout << "perfect match value is " << tempmatch << endl;
  
  
  double tempmatch = matchShapes(shape, shape2, CV_CONTOURS_MATCH_I2, 0);
  cout << "perfect match value is " << tempmatch << endl;

  double bestShapeMatch = 1000000;
  Point2d bestCenter;
  for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++) {
    Point2d center;
    const Moments moms = moments(Mat(contours[contourIdx]));
	
    // filter blobs which are too small
    double area = moms.m00;
    if ( area < 300 ) {
      continue;
    }
    Rect rect = boundingRect(contours[contourIdx]);
	
	//double Hu[7];
	//HuMoments(moms, Hu);
	//printf("%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\n", Hu[0], Hu[1], Hu[2], Hu[3], Hu[4], Hu[5], Hu[6]);
	double shapematch = matchShapes(contours[contourIdx], shape, CV_CONTOURS_MATCH_I2, 0);
	cout << "shape match = " << shapematch << endl;
		
    printf("Area is %.2f\n", area);
    printf("Rect left=%d, right=%d, top=%d, bottom=%d (origin at top/left)\n", 
      rect.x, rect.x + rect.width,
      rect.y, rect.y + rect.height );
    center = Point2d(moms.m10 / moms.m00, moms.m01 / moms.m00);
    if ( shapematch < bestShapeMatch ) {
		bestShapeMatch = shapematch;
		bestCenter = center;
	}
    printf("Located at (%.2f, %.2f)\n", center.x, center.y);
    circle( cleanedImage, center, 2, Scalar(0), 2, 8, 0);
    filteredContours.push_back(contours[contourIdx]);
    centers.push_back(center);
  }
  if ( bestShapeMatch < 100000 ) {
	  circle(cleanedImage, bestCenter, 3, Scalar(0,0,255), 2, 8, 0);
  }

  drawContours( cleanedImage, filteredContours, -1, Scalar(0,255,0) );
  namedWindow( cleaned_window, CV_WINDOW_AUTOSIZE );
  imshow( cleaned_window, cleanedImage );
}
