#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <limits>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core.hpp"
#include <fstream>

#define PORTNUMBER  9002 
#define DONOTKNOW 10000000

using namespace std;
using namespace cv;
// using namespace cv::gpu;

//added for further changes
const int iLowH = 0;
const int iHighH = 255;
const int iLowS = 62; 
const int iHighS = 255;
const int iLowV = 57;
const int iHighV = 255;
const int fr = 29;
const int xres = 1280;
const int yres = 720;
const double cameraAngle = 68.5;
const double yCameraAngle = (cameraAngle*9)/16;
double relativeBearing = DONOTKNOW;
double globalYAngle = DONOTKNOW;
pthread_mutex_t dataLock;
// forward declaration of functions
void *handleClient(void *arg);
void receiveNextCommand(char*, int);
void *capture(void *arg);
//import images
//gpu::setDevice(0);
const double _targetWidth = 3.31;
const double _targetHeight = 5.83;

int main(int argc , char** argv)
{
ofstream fout("output.txt");
	fout<<"main method started successfully"<<endl;
  int n, s;
  socklen_t len;
  int max;
  int number;
  struct sockaddr_in name;
  pthread_mutex_init(&dataLock, NULL);
  // create the socket
  if ( (s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  memset(&name, 0, sizeof(struct sockaddr_in));
  name.sin_family = AF_INET;
  name.sin_port = htons(PORTNUMBER);
  len = sizeof(struct sockaddr_in);

  // listen on all network interfaces
  n = INADDR_ANY;
  memcpy(&name.sin_addr, &n, sizeof(long));

  int reuse = 1;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
    perror("setsockopt(SO_REUSEADDR)");
    exit(1);
  }

  if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (const char*)&reuse, sizeof(reuse)) < 0) {
    perror("setsockopt(SO_KEEPALIVE)");
    exit(1);
  }

  // bind socket the network interface
  if (bind(s, (struct sockaddr *) &name, len) < 0) {
    perror("bind");
    exit(1);
  }

  // listen for connections
  if (listen(s, 5) < 0) {
    perror("listen");
    exit(1);
  }

	//FELIX EDIT FOR DEBUGGING
	fout<<"captured";
	
 
  pthread_t captureThreadId;
  int i = 3;
  int captureThread = pthread_create(&captureThreadId, NULL, capture, (void*)&i);

  // it is important to detach the thread to avoid memory leak
  pthread_detach(captureThreadId);

  while(true) {
	fout<<"anothyer"<<endl;
    // block until get a request
    int ns = accept(s, (struct sockaddr *) &name, &len);

    if ( ns < 0 ) {
      perror("accept");
      exit(1);
    }

    // each client connection is handled by a seperate thread since
    // a single client can hold a connection open indefinitely making multiple
    // data requests prior to closing the connection
    pthread_t threadId;
    int thread = pthread_create(&threadId, NULL, handleClient, (void*) &ns);
    // it is important to detach the thread to avoid a memory leak
    pthread_detach(threadId);
  } 
  fout.close();
  close(s);
  exit(0);
}

void *capture(void *arg) {  

  VideoCapture capture(1);
  capture.set(CV_CAP_PROP_FRAME_WIDTH, xres);
  capture.set(CV_CAP_PROP_FRAME_HEIGHT, yres);
  if(!capture.isOpened()) {
    cout << "Failed to connect to the camera." << endl;
  }
  Mat frame, dst, hsv, binary, tmpBinary;
  
  //Ideal shape of high goal reflective tape.
  vector<Point> shape;

  while(true) {
		
    capture >> frame;
    if(dst.empty()) {
      //cout << "failed to capture an image" << endl;
    }

    cvtColor(frame, hsv, CV_BGR2HSV);
    
     inRange(hsv, Scalar(iLowH,iLowS,iLowV), Scalar(iHighH,iHighS,iHighV), binary);

    std::vector < std::vector<Point> > contours;
    std::vector < std::vector<Point> > filteredContours;
    std::vector<Point2d> centers;
    tmpBinary = binary.clone();
    findContours(tmpBinary, contours, RETR_LIST, CHAIN_APPROX_NONE);
    tmpBinary.release();

    
    
    //double bestShapeMatch = DONOTKNOW;
    //Point2d pointOfBestShapeMatch;
    double minimumArea = 500;
    //bool foundBestMatch = false;    

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
    double aspectRatioTolerance = 0.4;
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
    if ( rectangularness < 0.4 ) {
      continue;
    }
	
    printf("Area is %.2f\n", area);
    center = Point2d(moms.m10 / moms.m00, moms.m01 / moms.m00);
    filteredContours.push_back(contours[contourIdx]);
    centers.push_back(center); 
    
    }
    //rectangle( frame, matchLoc, Point( matchLoc.x + src_d.cols , matchLoc.y + src_d.rows ), Scalar(255,0,0), 2, 8, 0 );
    
     double angle = DONOTKNOW;
     double yAngle = DONOTKNOW;
     if ( centers.size() == 2 ) {
      
       double centerX = (centers[0].x + centers[1].x)/2;
       double centerY = (centers[0].y + centers[1].y)/2;
       printf("Aim Point is (%.2f, %.2f)\n", centerX, centerY);
       Point2d aimPoint = Point2d(centerX, centerY);
       angle = (aimPoint.x - (xres/2))*cameraAngle/xres;
       yAngle = ((yres/2) - aimPoint.y )*yCameraAngle/yres;
       cout << "xangle = " << angle << endl;
       cout << "yAngle = " << yAngle << endl;
      //imshow(image_window, frame);
    }
  
    // obtain the lock and copy the data
    pthread_mutex_lock(&dataLock);
    relativeBearing = angle;
    globalYAngle = yAngle;
    pthread_mutex_unlock(&dataLock);
  }
}




void *handleClient(void *arg) {
  // printf("Thread starting\n");
  int ns = *((int*) arg);
  char sendbuffer[1024];
  char command[128];

  // start conversation with client
  while(true) {

    receiveNextCommand(command, ns);

    if ( strcmp(command, "STOP") == 0 ) {
      //printf("Received STOP command\n");
      break;
    } else if ( strcmp(command, "DATA") == 0 ) {
      //printf("Received DATA command\n");

      // obtain the lock and copy the data
      pthread_mutex_lock(&dataLock);
      double copyRelativeBearing = relativeBearing;
      double copyGlobalYAngle = globalYAngle;
      pthread_mutex_unlock(&dataLock);
      
      // the protocol will send an empty line when the data transfer is complete
      int sendbufferLen = -1;
      if ( copyRelativeBearing == DONOTKNOW ) {
        sendbufferLen = sprintf(sendbuffer, "\n");
      } else {
        sendbufferLen = sprintf(sendbuffer, "rb=%.1f\nya=%.1f\n\n", copyRelativeBearing, copyGlobalYAngle);
      }

      // write response to client
      write(ns, sendbuffer, sendbufferLen);
    } else {
      //printf("Received unknown command '%s'\n", command);
      break;
    }

  }
  close(ns);
  //printf("Thread ending\n");
  return 0;
}

void receiveNextCommand(char *command, int ns) {
  int receiveLength = read(ns, command, 1024);
  int commandLength = 0;
  while(commandLength < receiveLength) {
    char value = command[commandLength];
    if ( value == 0x0d || value == 0x0a ) {
      break;
    } 
    commandLength++;
  }

  // add the terminating 0 to mark the end of the string value in the char *
  command[commandLength] = 0;
}

