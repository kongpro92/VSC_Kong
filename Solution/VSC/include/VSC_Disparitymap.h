#pragma once
#ifndef DISPARITYMAP
#define DISPARITYMAP

#include <iostream>
#include <opencv2\core.hpp>
#include <vector>
#include <deque>
#include <algorithm>
#include <fstream>

using namespace cv;
using namespace std;

class Disparitymap
{
public:
    Disparitymap();
    Disparitymap(int camnum, int vxnum, int vynum, int vznum, float eps);
    ~Disparitymap();

    int Camnum;
    int xnum;       //voxel size x
    int ynum;       //voxel size y
    int znum;       //voxel size z
    Point2f**** vox;        //3D disparitymap
    Point2f*** tmpvox;
    bool**** errvox;        //validation error voxel
    float eps;
    int size2d;
    deque<deque<float*>>disparity;// each node 2D dispartiy map

    string Validation(int nb, double thr, double m_eps, double sigma); // nb,thr,m_eps -> MedianFilter | sigma,nb -> Smmothing
    void MedianFilter( int nb, double thr, double eps);
    void Linear3DInterpolation();
    void Smoothing(double sigma, int nb);
    void Write3DMap(string folderpath, int rcam, deque<int> ocam, deque<double> nodeX, deque<double> nodeY, deque<double> nodeZ);

};

#endif