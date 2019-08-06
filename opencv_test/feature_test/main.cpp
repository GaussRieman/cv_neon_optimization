//
//  main.cpp
//  feature_test
//
//  Created by Frank on 2018/7/27.
//  Copyright © 2018年 Frank. All rights reserved.
//

#include <iostream>
#include "opencv2/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"

using namespace std;
using namespace cv;

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    namedWindow("pic1");
    Mat image1 = imread("1.jpg", 1);
    Mat image2 = imread("2.jpg", 1);
    Mat gray1, gray2;
    cvtColor(image1, gray1, CV_RGBA2GRAY);
    cvtColor(image2, gray2, CV_RGBA2GRAY);
    
//    Mat corner;
//    cornerHarris(gray, corner, 3,3,0,01);
//    threshold(corner, corner, 0.001, 255, CV_THRESH_BINARY);
//    imshow("pic1", corner);
    
    vector<KeyPoint> key_points1;
    vector<KeyPoint> key_points2;
    Ptr<xfeatures2d::SURF> detector = xfeatures2d::SURF::create(2500);
    detector->detect(gray1, key_points1);
//    detector->
    

    
    
    waitKey();
    return 0;
}
