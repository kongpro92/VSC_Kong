#pragma once
#ifndef VSC_CAMERA_H
#define VSC_CAMERA_H

#include <fstream>

#include "../include/VSC_Position.h"
#include "../include/VSC_Matrix.h"
#include <string.h>
#include <limits.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Camera {
public:
    Camera(istream& is);
    Camera(string path);

    public:
    int width;
    int height;
    Mat P;

public:
    Position W2I(Position p);
    Position I2W(Position& p, double z);

public:
   
    double u, v;
    double l[4], r[2];
    double buffer4[4];
    double buffer12[12];
   

    void CalibrateCamera_(vector<Vec3f>originals, vector<Vec2f>centres, Size ImgSize);
    void CalibrateCamera_(vector<Vec3f>originals, vector<Vec2f>centres, Size ImgSize,string path);
    void WriteParameter(string path);
};
#endif // CAMERA_H
