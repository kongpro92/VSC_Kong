#include "main.h"

void main(int argc, char** argv) {
	readconfiguration(string(argv[1]), bd, camcnt, cam, pcnt, psize, isImportFF, FFfolderpath, isSteadyFlow, imgnum);
	Calbd.minx = -atof(argv[2]) / 2; gapx = atof(argv[2]) / 6;
	Calbd.miny = -atof(argv[3]) / 2; gapy = atof(argv[3]) / 6;
	Calbd.minz = -atof(argv[4]) / 2; gapz = atof(argv[4]) / 4;
	
	pos2d = new vector<Point2d>[camcnt];
	string saveDir = string(argv[6]) + "Parameter4Test";
	_mkdir(saveDir.c_str());

	random_device rd;
	mt19937 gen(rd());

	for (float error = 0; error <= atof(argv[5])+0.0000001; error += 0.05) {
		uniform_real_distribution<double> random_error(-error, error);
		
		for (int camcnt_ = 0; camcnt_ < cam.size(); camcnt_++) {
			for (int k = 0; k < NodeZnum; k++)for (int j = 0; j < NodeYnum; j++)for (int i = 0; i < NodeXnum; i++) {

				Vec3f posvec(Calbd.minx + gapx * i, Calbd.miny + gapy * j, Calbd.minz + gapz * k);
				Point3d pos(Calbd.minx + gapx * i, Calbd.miny + gapy * j, Calbd.minz + gapz * k);
				originals.push_back(posvec);
				pos3d.push_back(pos);
			}
			projectPoints(pos3d, cam[camcnt_].rvec, cam[camcnt_].tvec, cam[camcnt_].cameramatrix, cam[camcnt_].distCoeffs, pos2d[camcnt_]);
			//Add pixel error

			//point2d -> vec2d
			for (int ii = 0; ii < pos2d[camcnt_].size(); ii++) { 
				if (error != 0) {
					pos2d[camcnt_][ii].x += random_error(gen);
					pos2d[camcnt_][ii].y += random_error(gen);
				}
				centres.push_back(Vec2f(pos2d[camcnt_][ii].x, pos2d[camcnt_][ii].y)); 
			}
			pos2d[camcnt_].clear();

			string calpath = saveDir + "\\error_" + to_string(error).substr(0,4);
			_mkdir(calpath.c_str());
			calpath = calpath + "\\Caldata_" + to_string(camcnt_) + ".dat";
			cam[camcnt_].CalibrateCamera_(originals, centres, Size(cam[camcnt_].width, cam[camcnt_].height), calpath);

			originals.clear();
			centres.clear();
			pos3d.clear();
		}
	}
}