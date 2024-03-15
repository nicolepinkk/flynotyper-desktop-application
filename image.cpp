#include "image.h"

int ImAdjust(Mat image, Mat *adj_image, 
      double low_in[3], double high_in[3], 
      double low_out[3], double high_out[3], 
      double gamma) {

  double low_in2[3];
  double high_in2[3];
  double low_out2[3];
  double high_out2[3];
  double err_in[3];
  double err_out[3];
  int x, y, c;
  double val;

  for (c = 0; c < 3; c++) {
    if (low_in[c] < 0 || low_in[c] > 1 || high_in[c] < 0 || high_in[c] > 1 
        || low_out[c] < 0 || low_out[c] > 1 || high_out[c] < 0 || high_out[c] > 1 
        || low_in[c] > high_in[c]) {
      return -1;
    }
    low_in2[c] = low_in[c] * 255;
    high_in2[c] = high_in[c] * 255;
    low_out2[c] = low_out[c] * 255;
    high_out2[c] = high_out[c] * 255;
    err_in[c] = high_in[c] - low_in[c];
    err_out[c] = high_out[c] - low_in[c];
  }

  for (y = 0; y < image.rows; y++) {
    for (x = 0; x < image.cols; x++) {
      for (c = 0; c < 3; c++) {
        adj_image->at<Vec3b>(y,x)[c] = saturate_cast<uchar>(pow((image.at<Vec3b>(y,x)[c] - low_in2[c])/err_in[c], 1) *
          err_out[c] + low_out2[c]);
      }
    }
  }

  return 0;
}

Mat strelDisk(int Radius){ 
    Mat sel((2*Radius-1),(2*Radius-1),CV_8U,cv::Scalar(255));
    int borderWidth;
    switch (Radius){
        case 1: borderWidth = 0; break;
        case 3: borderWidth = 0; break;
        case 5: borderWidth = 2; break;
        case 7: borderWidth = 2; break;
        case 9: borderWidth = 4; break;
        case 11: borderWidth = 6; break;
        case 13: borderWidth = 6; break;
        case 15: borderWidth = 8; break;
        case 17: borderWidth = 8; break;
        case 19: borderWidth = 10; break;
        case 21: borderWidth = 10; break;
    }

    for (int i=0; i<borderWidth; i++){
        for (int j=0; j<borderWidth; j++){
                if (i+j<8){
                    sel.at<uchar>(i,j)=0;
                    sel.at<uchar>(i,sel.cols-1-j)=0;
                    sel.at<uchar>(sel.rows-1-i,j)=0;
                    sel.at<uchar>(sel.rows-1-i,sel.cols-1-j)=0;
                }
        }
    }
    
    return sel;
}
    
std::vector<Point> bhContoursCenter(const std::vector<std::vector<Point>>& contours) {
    std::vector<Point> result;
 
    for (int i=0; i < contours.size();i++)
    {
        Moments m = moments(contours[i],true);
        if (m.m10/m.m00 > 0 && m.m01/m.m00 > 0) {
            result.push_back( Point(m.m10/m.m00, m.m01/m.m00));
        }
    }

    return result;
}

std::vector<Point> bhFindLocalMaximum(Mat image,int neighbor=2, bool b_sem = false){
    Mat peak_img = image.clone();

    dilate(peak_img,peak_img,Mat(),Point(-1,-1),neighbor);
    peak_img = peak_img - image;

    Mat flat_img ;
    erode(image,flat_img,Mat(),Point(-1,-1),neighbor);
    flat_img = image - flat_img;
        
    threshold(peak_img,peak_img,0,255,cv::THRESH_BINARY);
    threshold(flat_img,flat_img,0,255,cv::THRESH_BINARY);
    bitwise_not(flat_img,flat_img);

    peak_img.setTo(Scalar::all(255),flat_img);
    bitwise_not(peak_img,peak_img);
    if (!b_sem) {
        dilate(peak_img,peak_img,Mat(),Point(-1,-1),3);
    }

    std::vector<std::vector<Point>> contours;
    findContours(peak_img,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);

    return bhContoursCenter(contours);
}
    
std::vector<Point> getRedPoints(Mat image) {
    int x, y, c;
    std::vector<Point> rpt;
        
    for (y = 0; y < image.rows; y++) {
        for (x = 0; x < image.cols; x++) {
            if (image.at<Vec3b>(y,x)[0] == 0 && image.at<Vec3b>(y,x)[1] == 0 &&
                image.at<Vec3b>(y,x)[2] == 255) {
                    rpt.push_back(Point(x,y));   
            }
        }
    }
        
    return rpt;
}
    
RotatedRect localizeEye(Mat image, bool b_sem, bool b_horiz, bool b_debug, bool b_write) {
    RotatedRect eps;
    double low_in[] = {0.1, 0.2, 0.4};
    double high_in[] = {0.8, 0.7, 0.9};
    double low_out[3] = {0, 0, 0};
    double high_out[3] = {1, 1, 1};
    int meanPix; 
    Mat adj_image = Mat::zeros(image.size(), image.type());
    Mat tmp_image = Mat::zeros(image.size(), image.type());
    Mat mid_image = Mat::zeros(image.size(), image.type());
    Mat bg_image;
    Mat img;
    double maxA = 0;
    int maxIdx = 0;
    Moments M;
    int eps_width;
    int eps_height;
        
    if (b_sem) {
        if (mean(image)[0] > 50) {
            ImAdjust(image, &adj_image, low_in, high_in, low_out, high_out, 1);
            meanPix = 150;
        } else {
            image.copyTo(adj_image); 
            meanPix = mean(image)[0] + 5;
        }
        cvtColor(adj_image,  bg_image, cv::COLOR_BGR2GRAY);
    } else {
        cvtColor(image, bg_image, cv::COLOR_BGR2GRAY); 
    }
            
    if (b_sem) {
        threshold( bg_image, tmp_image, meanPix, 255, 4);   
        cv::subtract(cv::Scalar::all(255), tmp_image, bg_image);
    } else {
        morphologyEx(bg_image, bg_image, 5, getStructuringElement(MORPH_ELLIPSE, Size(15,15), Point(-1,-1)));   
    }
            
    if (b_sem || image.rows * image.cols <= 800 * 600) {
        GaussianBlur(bg_image, tmp_image, Size(11,11), 0, 0, BORDER_DEFAULT);
    } else {
        GaussianBlur(bg_image, tmp_image, Size(9,9), 0, 0, BORDER_DEFAULT);
    }
    Sobel(tmp_image, mid_image, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT);
    convertScaleAbs(mid_image, tmp_image);
    Canny(tmp_image, mid_image, 60,100,3);
            
    if (b_debug) {
        namedWindow("bg image", WINDOW_AUTOSIZE);
        imshow("bg image", mid_image);
    }  
    if (b_write) {
        imwrite("bgImg.tif", bg_image);
        imwrite("edgeImg.tif", mid_image);
    }
            
    if (b_sem) {
        morphologyEx(mid_image, tmp_image, 3, getStructuringElement(MORPH_ELLIPSE, Size(25,25), Point(-1,-1))); 
    } else if (image.rows * image.cols <= 800 * 600) {
        morphologyEx(mid_image, tmp_image, 3, getStructuringElement(MORPH_ELLIPSE, Size(80,100), Point(-1,-1))); 
        erode(tmp_image, tmp_image, Mat(), Point(-1,1), 120);
        dilate(tmp_image, tmp_image, Mat(), Point(-1,1), 100);
    } else {
        morphologyEx(mid_image, tmp_image, 3, getStructuringElement(MORPH_ELLIPSE, Size(90,100), Point(-1,-1))); 
    }
            
    std::vector<std::vector<Point>> contours;
    findContours(tmp_image,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);
    img = Mat::zeros(tmp_image.rows, tmp_image.cols, CV_8UC1);
    drawContours(img, contours, -1, Scalar(255), cv::FILLED);
            
    if (b_debug) {
        namedWindow("contour image", WINDOW_AUTOSIZE);
        imshow("contour image", img); 
    }
    if (b_write) {
        imwrite("contour.tif", img);
    }
    
    for( int i = 0; i< contours.size(); i++ ) {
        M = moments(contours[i]);
        if (maxA < contourArea(contours[i])) {
            if (int(M.m10/M.m00) > 0 && int(M.m10/M.m00) < 0.75* img.cols &&
                    int(M.m01/M.m00) > 0 && int(M.m01/M.m00) < 0.75 * img.rows) {
                maxA = contourArea(contours[i]);
                maxIdx = i; 
            }
        }
    }
   
    if (contours[maxIdx].size() > 5) {      
        eps = fitEllipse(contours[maxIdx]);
        if (b_horiz) eps.angle = 90;
    } else {      
        eps = RotatedRect(Point2f(image.cols/2, image.rows/2), Size2f(image.cols,image.rows), 0);
    }
   
    eps_width = eps.size.width;
    eps_height = eps.size.height;

    if (!b_horiz && double(eps_width)/double(eps_height) > 0.67) {
        eps.center.x += (eps.size.width - double(eps.size.height)*0.67)/4;
        eps.size.width = int(double(eps.size.height) * 0.67);
    }
    
    if (eps.size.height > image.rows) {
        eps.size.height = image.rows;
        if (b_horiz) {
            eps.size.width = eps.size.width < int(double(eps.size.height) / 0.67) ? eps.size.width : int(double(eps.size.height) / 0.67);       
        } else {
            eps.size.width = int(double(eps.size.height) * 0.67);
        }
    }
            
    if (b_horiz && eps.size.width < eps.size.height) {
        eps_width += eps_height;
        eps_height = eps_width - eps_height;
        eps_width = eps_width - eps_height;
        eps.size.width = eps_width;
        eps.size.height = eps_height;
     }
        
     if (!b_horiz && eps.size.height < 2 * image.rows/3 ) {
         eps.size.height = int(double(image.cols) * 0.9);
         eps.size.width = int(double(eps.size.height) * 0.67);
     }
    
    return eps;
}
    
std::vector<Point> detectOmmatidia(Mat image, bool b_imagej = false, bool b_sem = false, bool b_horiz = false, bool b_debug = false, bool b_write = false) {
   Mat adj_image;
   Mat tmp_image;
   Mat gray_image;
   Mat mid_image;
   Mat bg_image = image.clone();
   Mat sem_image = image.clone();
   double low_in[] = {0, 0.1, 0.1};
   double high_in[] = {1, 0.9, 0.9};
   double low_out[3] = {0};
   double high_out[3] = {1};
   double nrows = image.rows;
   double ncols = image.cols;
   int meanPix;
   RotatedRect eps;
   std::vector<Point> cpt;
    std::vector<Point> cpt_clean;
    int neighbor = 2;
        
    if (b_imagej == false) {
       adj_image = Mat::zeros(image.size(), image.type());
       tmp_image = Mat::zeros(image.size(), image.type());
       mid_image = Mat::zeros(image.size(), image.type());
       gray_image = Mat::zeros(image.size(), image.type());
        
       eps = localizeEye(bg_image, b_sem, b_horiz, b_debug, b_write);
       
       if (b_debug) {
           ellipse(image, RotatedRect(eps.center, Size2f(eps.size.width,eps.size.height), 0), (0,0,255),8);
       }
       if (b_write) {
           imwrite("ROI.tif", image);
       }
            
       ImAdjust(image, &adj_image, low_in, high_in, low_out, high_out, 1);
            
       morphologyEx(adj_image, adj_image, 5, strelDisk(15));
       ImAdjust(adj_image, &mid_image, low_in, high_in, low_out, high_out, 1);
       cv::subtract(cv::Scalar::all(255), mid_image, adj_image);
            
       if (b_write) {
           imwrite("topHatImg.tif", adj_image);
       }

       cvtColor(adj_image, gray_image, cv::COLOR_BGR2GRAY); 
       if (!b_sem && image.rows * image.cols <= 800 * 600) {
           medianBlur(gray_image, tmp_image, 7);
       } else {
           medianBlur(gray_image, tmp_image, 13);
       }
            
       if (b_write) {
           imwrite("clean.tif", adj_image);
       }
            
       if (!b_sem) {
           cv::subtract(cv::Scalar::all(255), tmp_image, gray_image);
       } else {
           gray_image = tmp_image;
       }
        
       if (image.rows * image.cols <= (800 * 600)) {
           if (!b_sem) 
               neighbor = 7; 
           else 
               neighbor = 11;
       } else if (image.rows * image.cols <= (1600 * 1200)) {
               neighbor = 13;
       } else {
               neighbor = 25;
       }
            
       cpt = bhFindLocalMaximum(gray_image, neighbor, b_sem);
           
       for (int i = 0; i < cpt.size(); i ++) {
           if ((pow((cpt[i].x - eps.center.x)/(eps.size.width/2),2) + pow((cpt[i].y - eps.center.y)/(eps.size.height/2),2)) < 1) {
               cpt_clean.push_back(cpt[i]);
           } 
       }
            
       cpt_clean.push_back(Point(eps.center.x,eps.center.y));
           
       return cpt_clean;
    } else {
       return getRedPoints(image);
    }
}
