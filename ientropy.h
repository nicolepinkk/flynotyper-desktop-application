/* 
 * File:   ientropy.h
 * Author: qingyuwang
 *
 * Created on October 5, 2014, 6:03 PM
 */

#ifndef IENTROPY_H
#define	IENTROPY_H

#include <opencv2/opencv.hpp>
// #include <cv.h>

using namespace cv;
using namespace std;

double phynoScore(Point cpt_img, std::vector<Point> cpt, int z, int inc, double &de, double &ae, double &e, int rankType);

#endif	/* IENTROPY_H */

