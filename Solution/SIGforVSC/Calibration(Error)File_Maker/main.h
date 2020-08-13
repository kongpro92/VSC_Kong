#pragma once
#include <iostream>
#include <deque>
#include <vector>
#include <random>
#include "../include/SIG_Config.h"

using namespace std;

#define NodeXnum 7
#define NodeYnum 7
#define NodeZnum 5

vector<Vec3f>originals; vector<Vec2f>centres;
vector<Point3d> pos3d; vector<Point2d> *pos2d;
Boundary bd;
Boundary Calbd;
float gapx, gapy, gapz;
int camcnt;
deque<Camera> cam;
int pcnt;
double psize;
bool isImportFF; // Is Import Flow Field
string FFfolderpath; // Flow Field 데이터 폴더 경로
bool isSteadyFlow;
int imgnum; // 제작할 이미지 개수

