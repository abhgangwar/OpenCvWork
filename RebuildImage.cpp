#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

RNG rng(12345);
Scalar fillColor1 = Scalar( 255, 255, 255 );
Scalar fillColor2 = Scalar( 0, 0, 0 );

void customRebuild(Mat&, vector<vector<Point> >&, vector<Vec4i>&, bool);
void fillContour(Mat&, const Point**, int *, int, Scalar);
Scalar getFillColor(bool blacknWhite, int idx, int total = 0);

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
  findContours(inputGray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
  cout << "_hierarchy size " <<  hierarchy.size() << " contours size " << contours.size() << endl;
  Mat rebuild = Mat::zeros(cannyEdges.size(), CV_8UC3);
  rebuild = Scalar(255, 255, 255);
  customRebuild(rebuild, contours, hierarchy, false);

  namedWindow( "Rebuilt from contours", CV_WINDOW_NORMAL );
  imshow( "Rebuilt from contours", rebuild );

  Mat rebuildWithColors = Mat::zeros(cannyEdges.size(), CV_8UC3);
  rebuildWithColors = Scalar(255, 255, 255);
  customRebuild(rebuildWithColors, contours, hierarchy, true);

  namedWindow( "Contours filled with different colors", CV_WINDOW_NORMAL );
  imshow( "Contours filled with different colors", rebuildWithColors );

  waitKey(0);
  return(0);
}

void customRebuild(Mat& img, vector<vector<Point> >& _contours, vector<Vec4i>& _hierarchy, bool multipleColors)
{
  if(!_contours.size() || !_hierarchy.size()) return;
  int count = _hierarchy.size();
  Scalar color = Scalar( 0, 0, 0 );

  int i;
  for(i=0; i<count; ++i) {
    vector<Point> v = _contours[i];
    int size = _contours[i].size();
    vector<Point> *currentCon = &_contours[i];
    const Point **u = (const Point**)(&currentCon[0]);
    Scalar fillColor;
    if(multipleColors) {
      fillColor = getFillColor(false, i, count);
    } else {
      fillColor = getFillColor(true, i, 0);
    }
    fillPoly(img, u, &size, 1, fillColor, 8);
    int cSize = v.size();
    if(cSize) {
      Point prev = v[0];
      for(int j=0; j<(cSize-1); ++j) {
        line(img, prev, v[j+1], color, 2, 8, 0);
        prev = v[j+1];
      }
    }
  }

  // The colors used were HSV. Convert the color space.
  if(multipleColors) {
    cvtColor(img, img, COLOR_HSV2BGR_FULL);
  }
  return;
}

Scalar getFillColor(bool blacknWhite, int idx, int total)
{
  if(blacknWhite) {
    return (idx %2 == 0) ? fillColor1 : fillColor2;
  }
  else if(total == 0) {
    return fillColor1;
  }
  else {
    /**
     * To generate visually random color.
     * Refer to HSV color space https://en.wikipedia.org/wiki/HSL_and_HSV
     */
    // Hue is between [0, 180)
    float factor = float(180)/float(total);
    int hue = factor*(idx+1);
    // Saturation is between [0, 100)
    int saturation = 240 + rng.uniform(0, 15);
    // value is between [0, 100)
    int value = 240 + rng.uniform(0, 15);
    Scalar fillColor(hue, saturation, value);   
    return fillColor;
  }
}