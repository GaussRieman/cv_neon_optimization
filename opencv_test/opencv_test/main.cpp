//
//  main.cpp
//  opencv_test
//
//  Created by Frank on 2018/7/26.
//  Copyright © 2018年 Frank. All rights reserved.
//

#include "opencv2/opencv.hpp"
#include <iostream>
#include <Accelerate/Accelerate.h>
#include <math.h>
#include <time.h>

using namespace std;
using namespace cv;

typedef struct Pixel{
    int r;
    int g;
    int b;
} Pixel;

int bi_linear_inp(int l0x, int l0y, int l1x, int l1y, int r0x, int r0y, int r1x, int r1y){
    int r = 0;
    
//    int den = ()
    
    
    return r;
}

uchar maxInt4(uchar x0, uchar x1, uchar x2, uchar x3){
    uchar t0 = (x0 >= x1) ? x0 : x1;
    uchar t1 = (x2 >= x3) ? x2 : x3;
    return (t0 >= t1) ? t0 : t1;
}

uchar meanInt4(uchar x0, uchar x1, uchar x2, uchar x3){
    float t = (x0 + x1 + x2 + x3)/4.0;
    return uchar(round(t));  //四舍五入
}

uchar leftTopInt4(uchar x0, uchar x1, uchar x2, uchar x3){
    return x0;
}

Mat resizeByHalf(Mat input, uchar (*pfun)(uchar x0, uchar x1, uchar x2, uchar x3)){
    Mat output;
    int h_in = input.rows;
    int w_in = input.cols;
    int c = input.channels();
    
    int h_out = h_in/2;
    int w_out = w_in/2;
    output.create(h_out, w_out, CV_8UC(c));
    
    uchar *data_source = input.data;
    uchar *data_half = output.data;
    
    int bpl_source = w_in*3;
    int bpl_half = w_out*3;
    
    for (int i = 0; i < h_out; i++) {
        for (int j = 0; j < w_out; j++) {
            uchar *sr = data_source + i*2*bpl_source + j*2*c;
            uchar *hr = data_half + i*bpl_half + j*c;
            for (int k = 0; k < c; k++) {
                *(hr+k) = pfun(*(sr+k), *(sr+k+c), *(sr+k+bpl_source), *(sr+k+c+bpl_source));
            }
        }
    }
    
    return output;
}

Mat resizeByFloat(Mat input, float fx, float fy){
    Mat output;
    int h_in = input.rows;
    int w_in = input.cols;
    int c = input.channels();
    
    int h_out = int(h_in*fy);
    int w_out = int(w_in*fx);
    output.create(h_out, w_out, CV_8UC(c));
    
    uchar *data_source = input.data;
    uchar *data_half = output.data;
    
    int bpl_source = w_in*3;
    int bpl_half = w_out*3;
    
    int pos = 0;
    int sep = 0;
    float step = 0.0;
    float step_x = 1/fx;
    float step_y = 1/fy;
    
    for (int i = 0; i < h_out; i++) {
        for (int j = 0; j < w_out; j++) {
            sep = int(floor(step_y*i));
            uchar *sr = data_source + sep*bpl_source;
            uchar *hr = data_half + i*bpl_half +j*c;
            step = j*step_x;
            pos = int(floor(step))*c;
            for (int k = 0; k < c; k++) {
                *(hr+k) = *(sr+pos+k);
            }
        }
    }
    return output;
}

void resizeByNN(uchar *input, uchar *output, int height_in, int width_in, int channels, int height_out, int width_out){
    
    uchar *data_source = input;
    uchar *data_half = output;
    
    int bpl_source = width_in*3;
    int bpl_dst = width_out*3;
    
    int pos = 0;
    int sep = 0;
    uchar *sr = nullptr;
    uchar *hr = nullptr;
    float step = 0.0;
    float step_x = float(width_in)/float(width_out);
    float step_y = float(height_in)/float(height_out);
    
    for (int i = 0; i < height_out; i++) {
        for (int j = 0; j < width_out; j++) {
            sep = int(step_y*i);
            step = int(j*step_x);
            sr = data_source + sep*bpl_source;
            hr = data_half + i*bpl_dst +j*channels;
            pos = step*channels;
//            for (int k = 0; k < channels; k++) {
//                *(hr+k) = *(sr+pos+k);
//            }
            memcpy(hr, sr+pos, channels);
        }
    }
    return;
}

void resizeByTable(uchar *input, int channels, int length, int *table, uchar *output){
//    for (int i = 0; i < length; i++) {
//        memcpy(output+i*3, input+table[i], channels);
//    }
    int mass = 4*(length/4);
    for (int i = 0; i < mass; i+=4) {
        memcpy(output+(i+0)*3, input+table[i+0], channels);
        memcpy(output+(i+1)*3, input+table[i+1], channels);
        memcpy(output+(i+2)*3, input+table[i+2], channels);
        memcpy(output+(i+3)*3, input+table[i+3], channels);
    }

    for (int j = mass; j < length; j++) {
        memcpy(output+j*3, input+table[j], channels);
    }
    return;
}

int createTable(int *table, int width_in, int height_in, int channels, int width_out, int height_out){
    float step_x = float(width_in)/float(width_out);
    float step_y = float(height_in)/float(height_out);
    int length = width_out * height_out;
    int loc_y = 0;
    int loc_x = 0;
    int bytes_per_line = width_in * channels;
    
    for (int i = 0; i < length; i++) {
        loc_y = i/width_out;
        loc_x = i%width_out;
        table[i] = int(step_y*loc_y) * bytes_per_line + int(loc_x*step_x)*channels;
    }
    return 0;
}



int main()
{
    const char *img_name = "5.jpeg";
    Mat source = imread(img_name);
    int ws = source.cols;
    int hs = source.rows;
    printf("source = %d, %d\n", hs, ws);
    
    int cols_out = 661;
    int rows_out = 432;
    int length = rows_out * cols_out;
    int table[length];
//    float step_y = float(source.rows)/float(rows_out);
//    float step_x = float(source.cols)/float(cols_out);
//    printf("stepx = %f, stepy = %f\n", step_x, step_y);
//    int length = rows_out * cols_out;
//    int table[length];
//    int loc_y = 0;
//    int loc_x = 0;
//    int channels = source.channels();
//    int bytes_per_line = source.cols * source.channels();
//
//    for (int i = 0; i < length; i++) {
//        loc_y = i/cols_out;
//        loc_x = i%cols_out;
//        table[i] = int(step_y*loc_y) * bytes_per_line + int(loc_x*step_x)*channels;
//    }

    createTable(table, source.cols, source.rows, source.channels(), cols_out, rows_out);

    struct timeval t1, t2, t3, t4, t5, t6;
    double time = 0.0;
    gettimeofday(&t1, NULL);
    Mat half_img = resizeByHalf(source, &leftTopInt4);
    int wh = half_img.cols;
    int hh = half_img.rows;
    printf("half_img = %d, %d\n", hh, wh);
    gettimeofday(&t2, NULL);
    time = 1000*(t2.tv_sec - t1.tv_sec) + (double)(t2.tv_usec - t1.tv_usec)/1000;
    printf("frank resizeByHalf = %lf\n", time);

    
    Mat resize_img = resizeByFloat(source, 800.0/1323.0, 600.0/864.0);
    gettimeofday(&t3, NULL);
    time = 1000*(t3.tv_sec - t2.tv_sec) + (double)(t3.tv_usec - t2.tv_usec)/1000;
    printf("frank resizeByFloat = %lf\n", time);
//    int wr = resize_img.cols;
//    int hr = resize_img.rows;
//    printf("resize_img = %d, %d\n", hr, wr);
    
    
    Mat new_img = Mat(rows_out, cols_out, CV_8UC3);
    resizeByNN(source.data, new_img.data, source.rows, source.cols, source.channels(), new_img.rows, new_img.cols);
    gettimeofday(&t4, NULL);
    time = 1000*(t4.tv_sec - t3.tv_sec) + (double)(t4.tv_usec - t3.tv_usec)/1000;
    printf("frank resizeByNN = %lf\n", time);
//    int wn = new_img.cols;
//    int hn = new_img.rows;
//    printf("new_img = %d, %d\n", hn, wn);
    
    
    Mat img0;
    cv::resize(source, img0, cv::Size(cols_out,rows_out));
    gettimeofday(&t5, NULL);
    time = 1000*(t5.tv_sec - t4.tv_sec) + (double)(t5.tv_usec - t4.tv_usec)/1000;
    printf("frank cv::resize = %lf\n", time);
    
    Mat img1 = Mat(rows_out, cols_out, CV_8UC3);
    resizeByTable(source.data, source.channels(), rows_out*cols_out, table, img1.data);
    gettimeofday(&t6, NULL);
    time = 1000*(t6.tv_sec - t5.tv_sec) + (double)(t6.tv_usec - t5.tv_usec)/1000;
    printf("frank resizeByTable = %lf\n", time);

//    imwrite("half.jpg", half_img);
//    imwrite("resize.jpg", resize_img);
    

    namedWindow("source");
    imshow("source", source);
//    namedWindow("half");
//    imshow("half", half_img);
    namedWindow("resize");
    imshow("resize", new_img);
    namedWindow("new");
    imshow("new", img1);
    waitKey();
    
    return 0;
}
