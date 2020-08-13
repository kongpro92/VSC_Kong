#pragma once
#include <deque>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

#include <VSC_Camera.h>
#include <VSC_Position.h>
#include <VSC_Configuration.h>
#include <VSC_ParticleFinder.h>

using namespace std;
using namespace cv;

#ifndef VSC_COMMON_H
#define VSC_COMMON_H

void SetConsole(string title);
void SetDisplay(bool &display);
void SetReferenceCamera(int &rcam_, deque<int> &ocam_);
void SetEpsilon(float &eps1_, float &eps2_);
void CopyMatToArr(Mat mat, int ** dst);
int** CreateArr(int height, int width);
void WritePosition_B(string path, deque<Position> &p);
void WritePosition(string path, deque<Position> &p);
void Traigulation4cam(Position candi, Camera cam1, Camera cam2, Camera cam3, Camera cam4, Position &dst);
void Traigulation2cam(ParticleFinder finder1, int idx1, ParticleFinder finder2, int idx2, Camera cam1, Camera cam2, Position &dst);
float subpixel(float a1, float a2, float a3);
#endif // !VSC_COMMOM_H
