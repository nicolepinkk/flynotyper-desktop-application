/* 
 * File:   image.h
 * Author: qingyuwang
 *
 * Created on September 18, 2014, 5:54 PM
 */

#ifndef IMAGE_H
#define	IMAGE_H

#include <opencv2/opencv.hpp>
// #include <cv.h>
//#include <highgui.h>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

std::vector<Point> detectOmmatidia(Mat image, bool b_imagej, bool b_sem, bool b_horiz, bool b_debug, bool b_write);

#endif	/* IMAGE_H */

