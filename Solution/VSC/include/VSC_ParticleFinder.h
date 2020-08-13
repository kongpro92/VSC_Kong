#pragma once
#ifndef VSC_PARTICLEFINDER_H
#define VSC_PARTICLEFINDER_H

#include <string>
#include <deque>

using namespace std;

class ParticleFinder {
public:
    ParticleFinder(int**& p, int rows, int cols) : pixels(p), rows(rows), cols(cols) {};
	ParticleFinder(int rows, int cols) : rows(rows), cols(cols) {}
    // destructor
    ~ParticleFinder() {};
    


    int** pixels;
    int rows;
    int cols;

    // store vectors of the x and y coordinates of the particles
    deque<double> x;
    deque<double> y;
    deque<int> inten;
    // helper function
    bool IsLocalMax(int r, int c);
    void FindParticle2DCenter(int depth, int threshold);
	void readParticle2DPosition(string path);
    };

#endif 