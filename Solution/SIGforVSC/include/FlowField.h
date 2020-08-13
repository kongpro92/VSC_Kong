#ifndef FLOWFIELD_H
#define FLOWFIELD_H
#include "Particle.h"
#include "boundary.h"
#include "../include/Blocking.h"
#include <fstream>
#include <string>
#include <sstream>

class FlowField
{
public:
	FlowField();
	~FlowField();

	deque<Point3d> pt;
	deque<Point3d> velo;

	static bool ffimport;
	static bool Steady;
	void ApplyFlowField(Boundary bd, Block bl, Particle &par);
	bool isSteady() { return Steady; }
	bool isFFimport() { return ffimport; }
	void importffdata(string path);
	
private:

};


#endif // !FLOWFIELD_H
