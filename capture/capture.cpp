#include <iostream>
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

const char* capture_window = "Capture Window";
const char* binary_window = "Binary Image";
const char* clean_window = "Cleaned Image";

int main( ) {
  VideoCapture capture(1);
  // want 1920X1080 ?
  capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  capture.set(CV_CAP_PROP_FPS, 10);
  if(!capture.isOpened()) {
    cout << "Failed to connect to the camera." << endl;
  }
  namedWindow(capture_window, CV_WINDOW_AUTOSIZE);
  namedWindow(binary_window, CV_WINDOW_AUTOSIZE);
  namedWindow( clean_window, CV_WINDOW_AUTOSIZE );
  Mat frame, hsv, binary, tmpBinary, clean;
  while(true) {
    capture >> frame;
    if(frame.empty()) {
      cout << "failed to capture an image" << endl;
      return -1;
    }
    imshow(capture_window, frame);
    
    cvtColor(frame, hsv, CV_BGR2HSV);
    inRange(hsv, Scalar(30,22,158), Scalar(100,255,255), binary);
    imshow(binary_window, binary);

    std::vector < std::vector<Point> > contours;
    std::vector < std::vector<Point> > filteredContours;
    tmpBinary = binary.clone();
    findContours(tmpBinary, contours, RETR_LIST, CHAIN_APPROX_NONE);
    tmpBinary.release();
    cvtColor( binary, clean, CV_GRAY2RGB );
    clean.setTo(Scalar(255,255,255));

    for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++) {
      Point2d center;
      Moments moms = moments(Mat(contours[contourIdx]));

      // filter blobs which are too small
      double area = moms.m00;
      if ( area < 500 ) {
        continue;
      }
      center = Point2d(moms.m10 / moms.m00, moms.m01 / moms.m00);
      circle(clean, center, 2, Scalar(0), 2, 8, 0);
      filteredContours.push_back(contours[contourIdx]);
    }

    drawContours( clean, filteredContours, -1, Scalar(0,255,0) );
    imshow(clean_window, clean);
  }
  return(0);
}
