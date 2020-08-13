#ifndef PARTICLE_H
#define PARTICLE_H
#include <opencv2/core.hpp>
#include <vector>
#include <deque>
#include <random>
#include "../include/boundary.h"
#include "../include/Camera.h"
#include "../include/Blocking.h"
#include <iostream>

using namespace cv;
using namespace std;

class Particle
{
public:
	Particle();
	Particle(int num_, double size_);
	~Particle();

	vector<Point3d> pos3d;
	vector<vector<Point2d>> pos2d;
	int num;
	double size;

	void MakeRandomPosition(Boundary bd, Block bl);
	void MoveRandomPositionYZ(Boundary bd, Block bl, int idx);
	void Project2Image(deque<Camera> cam);

private:

};

#endif // !PARTICLE_H


