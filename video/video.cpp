#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
 
using namespace cv;
using namespace std;

const char* source_window = "Source";
const char* binary_window = "Binary Image";
const char* clean_window = "Cleaned Image";

/** @function main */
int main( int, char** argv )
{
  char* image_dir = argv[1]; 
  int iLowH = 31;
  int iHighH = 105;
  int iLowS = 22;
  int iHighS = 255;
  int iLowV = 158;
  int iHighV = 255;
  char source_name[250];
  char binary_name[250];
  char clean_name[250];
  namedWindow ( source_window, CV_WINDOW_AUTOSIZE );
  //namedWindow ( binary_window, CV_WINDOW_AUTOSIZE );
  namedWindow ( clean_window, CV_WINDOW_AUTOSIZE );
  std::vector<Point> shape;
  shape.push_back(Point2d(0,0));
  shape.push_back(Point2d(0,12));
  shape.push_back(Point2d(2,12));
  shape.push_back(Point2d(2,2));
  shape.push_back(Point2d(18,2));
  shape.push_back(Point2d(18,12));
  shape.push_back(Point2d(20,12));
  shape.push_back(Point2d(20,0));
  shape.push_back(Point2d(0,0));

  Mat source, hsv, binary, clean;
  std::vector<Point> recentTargets;
  for ( int i = 30; i < 600; i++ ) {
    printf("Frame %d\n", i);
    sprintf(source_name, "%s/%dsource.jpg", image_dir, i);

    std::vector < std::vector<Point> > contours;
    std::vector < std::vector<Point> > filteredContours;
    std::vector<Point2d> centers;

    /// Load source image and convert it to gray
    source = imread( source_name, 1 );
    cvtColor(source, hsv, CV_BGR2HSV);
 
    inRange( hsv, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), binary );

    Mat tmpBinaryImage = binary.clone();
    findContours(tmpBinaryImage, contours, RETR_LIST, CHAIN_APPROX_NONE);
    cvtColor(binary, clean, CV_GRAY2RGB);
    clean.setTo(Scalar(255,255,255));
    double bestShapeMatch = 1000000;
    Point2d bestCenter;

    for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++) {
      Point2d center;
      const Moments moms = moments(Mat(contours[contourIdx]));
      double area = moms.m00;

      if ( area < 400 ) {
        continue;
      }

      double shapematch = matchShapes(contours[contourIdx], shape, CV_CONTOURS_MATCH_I2, 0);

      if ( shapematch > 5 ) {
        continue;
      }

      center = Point2d(moms.m10/moms.m00, moms.m01/moms.m00);

      if ( shapematch < bestShapeMatch ) {
        bestShapeMatch = shapematch;
        bestCenter = center;
      }
      circle(clean, center, 2, Scalar(0), 2, 8, 0);
      filteredContours.push_back(contours[contourIdx]);
      centers.push_back(center);
    }
    if ( recentTargets.size() >= 10 ) {
      recentTargets.erase( recentTargets.begin() );
    } 
    if ( bestShapeMatch < 1000000 ) {
      circle(clean, bestCenter, 2, Scalar(0,0,255), 2, 8, 0);
      circle(clean, bestCenter, 7, Scalar(0,0,255), 2, 8, 0);
    }
    double tolerance = 50;
    double sum = 0;
    double good_count = 0;
    for ( size_t rtIdx = 0; rtIdx < recentTargets.size(); rtIdx++ ) {
      double x = recentTargets[rtIdx].x;
      if ( x >= 0 ) {
        sum += x;
        good_count++;
      }
    }
    // we have 8 or 9 good heading values
    if ( good_count >= 6 && bestShapeMatch < 1000000 ) {
      double averageX = sum / good_count;
      if ( abs(bestCenter.x - averageX) < tolerance ) {
        recentTargets.push_back(bestCenter);
        good_count++;
      } else {
        // discard as a spurious measurement
        recentTargets.push_back(Point2d(-1, -1));
      }
      if ( good_count > 5 ) {
//        line( clean, Point2d(averageX-10, 100), Point2d(averageX+10, 100), Scalar(128,128,128), 1, 8, 0);
 //       line( clean, Point2d(averageX-10, 200), Point2d(averageX+10, 200), Scalar(128,128,128), 1, 8, 0);
        // green line
  //      line( clean, Point2d(averageX, 200), Point2d(averageX, 100+10*(10 - good_count)), Scalar(4,106,38), 2, 8, 0);
      }
    } else if ( bestShapeMatch < 1000000 ) {
      recentTargets.push_back(bestCenter);
      good_count++;
    } else {
      recentTargets.push_back(Point2d(-1, -1));
    }
    drawContours(clean, filteredContours, -1, Scalar(0,255,0) );

    imshow(source_window, source);
    //imshow(binary_window, binary);
    imshow(clean_window, clean);
 
    waitKey(100);
  }
  
  return(0);
}
