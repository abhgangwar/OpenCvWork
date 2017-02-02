#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

void customRebuild(Mat&, vector<vector<Point> >&, vector<Vec4i>&);

/** @function main */
int main( int argc, char** argv )
{
  Mat input, inputGray;
  // Reading the input file.
  input = imread( argv[1], 1 );

  // Convert the image from RGB to gray and blur it
  cvtColor( input, inputGray, CV_BGR2GRAY );
  blur( inputGray, inputGray, Size(3,3) );

  // Display the gray image 
  namedWindow( "Original Image - Gray fromat", CV_WINDOW_NORMAL );
  imshow( "Original Image - Gray fromat", inputGray );

  Mat cannyEdges;
  int threshold = 100;
  Canny(inputGray, cannyEdges, threshold, threshold*2);
  namedWindow("CannyEdges", CV_WINDOW_NORMAL);
  imshow("CannyEdges", cannyEdges);

  //Finding contours
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  findContours(cannyEdges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
  Mat rebuild = Mat::zeros(cannyEdges.size(), CV_8UC3);
  // rebuild[:] = (255, 255, 255)
  rebuild = Scalar(255, 255, 255);
  namedWindow("Rebuild", CV_WINDOW_NORMAL);
  imshow("Rebuild", rebuild);

  customRebuild(rebuild, contours, hierarchy);

  waitKey(0);
  return(0);
}

void customRebuild(Mat& img, vector<vector<Point> >& _contours, vector<Vec4i>& _hierarchy)
{
  // vector<CvSeq> contours;
  if(!_contours.size() || !_hierarchy.size()) return;
  int count = _hierarchy.size();
  Scalar color = Scalar( 0, 0, 0 );

  for(int i=0; i<count; ++i) {
    vector<Point> v = _contours[i];
    int cSize = v.size();
    if(cSize) {
      Point prev = v[0];
      for(int j=0; j<(cSize-1); ++j) {
        line(img, prev, v[j+1], color, 2, 8, 0);
        prev = v[j+1];
      }
    }
  }

  namedWindow( "MyWindow", CV_WINDOW_NORMAL );
  imshow( "MyWindow", img );
  return;
}