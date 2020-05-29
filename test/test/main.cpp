//
//  main.cpp
//  test
//
//  Created by 徐亦燊 on 2020/2/28.
//  Copyright © 2020 徐亦燊. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;
using namespace std;

#include<opencv2/opencv.hpp>
#include<iostream>
using namespace cv;
using namespace std;
double Distance(float * X, float * Y, int N) {
    double sum = 0;
    for (int i = 0; i < N; i++)
    {
        sum += abs(*(X + i) - *(Y + i));
    }
    return sum;
}
void HIS(Mat&MAG, Mat&ANGLE, int BLOCKSize,
    float * hist, int biasX,
    int biasY, int bias)
{
    int bias_PicX = biasX * BLOCKSize, bias_PicY = biasY * BLOCKSize;
    for (int i = bias_PicX; i < BLOCKSize + bias_PicX; i++)
    {
        for (int j = bias_PicY; j < BLOCKSize + bias_PicY; j++)
        {
            int range = (ANGLE.at<float>(j, i)) / 40;
            *(hist + range + bias) += MAG.at<float>(j, i);
        }
    }
}
//画出所确定的追踪物体
void DRAW(Mat & src, int blockSize, int nX, int nY, float * hist)
{
    Mat gx, gy;
    Mat mag, angle;
    Sobel(src, gx, CV_32F, 1, 0, 1);
    Sobel(src, gy, CV_32F, 0, 1, 1);
    cartToPolar(gx, gy, mag, angle, true);
    int bias = 0;
    for (int i = 0; i < nY; i++)
    {
        for (int j = 0; j < nX; j++)
        {
            HIS(mag, angle, blockSize, hist, j, i, bias);
            bias += 9;
        }
    }
}
int main()
{
    Mat refMat = imread("/Users/xuyishen/Desktop/sa jiganyouhaximeiyouga/2020.5.29/chase/template.png", 0);
    Mat rlMat = imread("/Users/xuyishen/Desktop/sa jiganyouhaximeiyouga/2020.5.29/chase/img.png", 0);
    int bSize = 12;
    int nX = refMat.cols / bSize;
    int nY = refMat.rows / bSize;
    int bins = 9 * nX*nY;
    //记录最小和对应位置
    double MinR = 1e30;
    int nMinX = 0;
    int nMinY = 0;
    int nHeight = rlMat.rows;
    int nWidth = rlMat.cols;
    //高、宽
    int nTplHeight = refMat.rows;
    int nTplWidth = refMat.cols;
    float * REF_HIST = new float[bins];
    memset(REF_HIST, 0, sizeof(float)*bins);
    DRAW(refMat, bSize, nX, nY, REF_HIST);
    std::cout << "Please wait,calculating..." << endl;
    for (int i = 0; i < nHeight - nTplHeight + 1; i++)
    {
        for (int j = 0; j < nWidth - nTplWidth + 1; )
        {
            Rect rect(j, i, nTplWidth, nTplHeight);
            Mat image_roi = rlMat(rect);
            float * ROI_HIST = new float[bins];
            memset(ROI_HIST, 0, sizeof(float)*bins);
            DRAW(image_roi, bSize, nX, nY, ROI_HIST);
            double O_Dis = Distance(REF_HIST, ROI_HIST, bins);
            if (O_Dis < MinR)
            {MinR = O_Dis;
                nMinX = j;
                nMinY = i;
            }
            delete[]  ROI_HIST;
            j = j + 5;
        }
        i = i + 5;
    }
    delete[] REF_HIST;
    cout << "X位置:" << nMinX << endl;
    cout << "Y位置:" << nMinX;
    Rect rect(nMinX, nMinY, nTplWidth, nTplHeight);
    rectangle(rlMat, rect, Scalar(0, 255, 255), 2, 10, 0);
    imshow("锁定后物体", rlMat);
    waitKey(0);
    return 0;
}
