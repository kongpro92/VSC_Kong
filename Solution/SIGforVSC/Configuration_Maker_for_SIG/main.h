#include <iostream>
#include <deque>
#include "../include/SIG_Config.h"

using namespace std;

#define CameraParam 27

string argload(int argc, char** argv);


Boundary bd;
int camcnt;
deque<Camera> cam;
int pcnt;
double psize;
bool isImportFF; // Is Import Flow Field
string FFfolderpath; // Flow Field 데이터 폴더 경로
bool isSteadyFlow;
int imgnum; // 제작할 이미지 개수


string argload(int argc, char** argv) {
    if ((argc-12)% CameraParam == 0) {
        bd.maxx = atof(argv[1])/2; bd.minx = -bd.maxx;
        bd.maxy = atof(argv[2])/2; bd.miny = -bd.maxy;
        bd.maxz = atof(argv[3])/2; bd.minz = -bd.maxz;
        camcnt = atoi(argv[4]);
		int arg = 0;
		for (int cnt = 0; cnt < camcnt; cnt++) {
			arg = 5 + cnt* CameraParam;
			cam.push_back(
				*new Camera(atoi(argv[arg +  0]), atoi(argv[arg +  1]),
					        atof(argv[arg +  2]), atof(argv[arg +  3]), atof(argv[arg +  4]), atof(argv[arg +  5]),
							atof(argv[arg +  6]), atof(argv[arg +  7]), atof(argv[arg +  8]),
							atof(argv[arg +  9]), atof(argv[arg + 10]), atof(argv[arg + 11]),
							atof(argv[arg + 12]), atof(argv[arg + 13]), atof(argv[arg + 14]), atof(argv[arg + 15]),
							atof(argv[arg + 16]), atof(argv[arg + 17]), atof(argv[arg + 18]), atof(argv[arg + 19]),
							atof(argv[arg + 20]), atof(argv[arg + 21]), atof(argv[arg + 22]), atof(argv[arg + 23]),
							atof(argv[arg + 24]), atof(argv[arg + 25]),
							atof(argv[arg + 26])));
		}
        pcnt  = atoi(argv[arg + 27]);
        psize = atof(argv[arg + 28]);
		isImportFF = atoi(argv[arg + 29]);
		FFfolderpath = string(argv[arg + 30]);
		isSteadyFlow = atoi(argv[arg + 31]);
		imgnum = atoi(argv[arg + 32]);

		return string(argv[arg + 33]);
    }

	return string("");
}
