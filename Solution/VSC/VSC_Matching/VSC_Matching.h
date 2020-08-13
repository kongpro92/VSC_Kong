#pragma once

#ifndef VSC_MATCHING_H
#define VSC_MATCHING_H

#include <iostream>
#include <direct.h>
#include <opencv2/opencv.hpp>

#include <VSC_Configuration.h>
#include <VSC_ParticleFinder.h>
#include <VSC_Position.h>
#include <VSC_Common.h>
#include <VSC_Matrix.h>
#include <VSC_Camera.h>
#include <VSC_Logs.h>
#include <omp.h>

#define RangeTestPr(i)  conf.Xmin < candTmp[tid].Xr##i && candTmp[tid].Xr##i < conf.Xmax && conf.Ymin < candTmp[tid].Yr##i && candTmp[tid].Yr##i < conf.Ymax && conf.Zmin < candTmp[tid].Zr##i && candTmp[tid].Zr##i < conf.Zmax

using namespace cv;
using namespace std;

int rcam;
bool display;
float eps1;
float eps2;

Configuration conf;
deque<int> ocam;
deque<int **> img;
deque<Camera> cam;
deque<ParticleFinder> finder;

#define ThreadNum 6
#define root 1.4142

deque<Position> mPs;			// Matched Positions
deque<Position> cands;
Position tmp[ThreadNum];
Position Rmin[ThreadNum];
Position Rmax[ThreadNum];
Position RminToImg1[ThreadNum];
Position RmaxToImg1[ThreadNum];
Position toGetRef[ThreadNum];
Position OptPosition_cam1[ThreadNum];
Position OptPosition_cam2[ThreadNum];
Position OptPosition_cam3[ThreadNum];
Position toGetFinal[ThreadNum];
Position FinalOpt1[ThreadNum];
Position FinalOpt2[ThreadNum];
Position FinalOpt3[ThreadNum];
Position candTmp[ThreadNum];
Position Ref[ThreadNum];
Position ROI1[ThreadNum], ROI2[ThreadNum];
Position ROI3[ThreadNum], ROI4[ThreadNum];
Position ROI5[ThreadNum], ROI6[ThreadNum];
float a1[ThreadNum], m1[ThreadNum], c1[ThreadNum];
float a2[ThreadNum], b2[ThreadNum];
float a3[ThreadNum], b3[ThreadNum];
float dist1[ThreadNum], dist2[ThreadNum], dist3[ThreadNum];
deque<Position> *tmpWorldPosition = new deque<Position>[ThreadNum];

void SetCamParams()
{
	for (int camcnt = 0; camcnt < conf.CalibrationFilePath.size(); camcnt++){
		cam.push_back(Camera(conf.CalibrationFilePath[camcnt]));
	}
}
void VSC_Matching()
{
	omp_set_num_threads(ThreadNum);
	cout << endl;
	cout << "\t                         _____________________________________________________\n";
	cout.fill(' '), cout.width(9), cout << "\t\t\t\t", cout.fill(' '), cout.width(9), cout << "| RefCam |";
	cout << "  Cam" << to_string(ocam[0] + 1) << "  |";
	cout << "  Cam" << to_string(ocam[1] + 1) << "  |";
	cout << "  Cam" << to_string(ocam[2] + 1) << "  |";
	cout << " Matchs" << " |";
	cout << "  Time" << "  |\n";
	cout << "\t------------------------|--------|--------|--------|--------|--------|--------|\n";

	for (int fcnt = 0; fcnt < conf.ImagePath[0].size(); fcnt++) {
		time_t s = clock();
		cout.fill(' '), cout.width(9), cout << to_string(fcnt + 1) << " Frame Particle detect | ";
		for (int camcnt = 0; camcnt < conf.Camcnt; camcnt++) {
			/*Mat tmp = imread(conf.ImagePath[camcnt][fcnt], IMREAD_GRAYSCALE);

			if (fcnt == 0) img.push_back(CreateArr(tmp.rows, tmp.cols));

			CopyMatToArr(tmp, img[camcnt]);

			ParticleFinder find(img[camcnt], tmp.rows, tmp.cols);
			find.FindParticle2DCenter(255, 30); finder.push_back(find);*/
			ParticleFinder find(512, 512);
			find.readParticle2DPosition(conf.ProjFolder + "Cam" + to_string(camcnt) + "_Position\\" + to_string(fcnt + 100000) + ".dat");
			finder.push_back(find);
			cout.fill(' '); cout.width(6); cout << to_string(find.x.size()) << " | ";
		}


#pragma region Check Feasibility for all possible particle matching
#pragma omp parallel for
		for (int ir = 0; ir < finder[rcam].x.size(); ir++) {
			int tid = omp_get_thread_num();

			tmp[tid] = Position(finder[rcam].x[ir], finder[rcam].y[ir], 0);
			Rmin[tid] = cam[rcam].I2W(tmp[tid], conf.Zmin);
			Rmax[tid] = cam[rcam].I2W(tmp[tid], conf.Zmax);
			RminToImg1[tid] = cam[ocam[0]].W2I(Rmin[tid]);
			RmaxToImg1[tid] = cam[ocam[0]].W2I(Rmax[tid]);

			m1[tid] = (RmaxToImg1[tid].y - RminToImg1[tid].y) / (RmaxToImg1[tid].x - RminToImg1[tid].x);
			a1[tid] = -m1[tid]; c1[tid] = m1[tid] * RminToImg1[tid].x - RminToImg1[tid].y;

#pragma region Display refCam
			Mat refCam = imread(conf.ImagePath[rcam][fcnt]);
			if (display) {
				circle(refCam, Point2f(finder[rcam].x[ir], finder[rcam].y[ir]), 2, Scalar(0, 255, 0), 1);
				imshow("Ref Cam", refCam);
				moveWindow("Ref Cam", 0, 0);
				waitKey(1);
			}
#pragma endregion

#pragma region ROI Setting
			if (RminToImg1[tid].x > RmaxToImg1[tid].x) { ROI1[tid].x = RmaxToImg1[tid].x - eps1, ROI2[tid].x = RminToImg1[tid].x + eps1; }
			else { ROI1[tid].x = RminToImg1[tid].x - eps1, ROI2[tid].x = RmaxToImg1[tid].x + eps1; }
			if (RminToImg1[tid].y > RmaxToImg1[tid].y) { ROI1[tid].y = RmaxToImg1[tid].y - eps1, ROI2[tid].y = RminToImg1[tid].y + eps1; }
			else { ROI1[tid].y = RminToImg1[tid].y - eps1, ROI2[tid].y = RmaxToImg1[tid].y + eps1; }
#pragma endregion

			for (int i1 = 0; i1 < finder[ocam[0]].x.size(); i1++) {
				if (!(ROI1[tid].x < finder[ocam[0]].x[i1] && ROI2[tid].x > finder[ocam[0]].x[i1])) continue;
				if (!(ROI1[tid].y < finder[ocam[0]].y[i1] && ROI2[tid].y > finder[ocam[0]].y[i1])) continue;

				dist1[tid] = abs(a1[tid] * finder[ocam[0]].x[i1] + finder[ocam[0]].y[i1] + c1[tid]) / sqrt(a1[tid] * a1[tid] + 1);
				if (dist1[tid] > eps1) continue;

#pragma region Display firstCam
				Mat firstCam = imread(conf.ImagePath[ocam[0]][fcnt]);
				if (display) {
					line(firstCam, Point2f(ROI1[tid].x, ROI1[tid].y), Point2f(ROI2[tid].x, ROI2[tid].y), Scalar(0, 0, 255), 1);
					circle(firstCam, Point2f(finder[ocam[0]].x[i1], finder[ocam[0]].y[i1]), 2, Scalar(0, 255, 0), 1);
					imshow("First Cam", firstCam);
					moveWindow("First Cam", 1030, 0);
					waitKey(100);
				}
#pragma endregion

				Traigulation2cam(finder[rcam], ir, finder[ocam[0]], i1, cam[rcam], cam[ocam[0]], OptPosition_cam1[tid]);
				Position tmp = Position(finder[rcam].x[ir], finder[rcam].y[ir], 0);
				OptPosition_cam1[tid] = cam[rcam].I2W(tmp, OptPosition_cam1[tid].z);

				OptPosition_cam2[tid] = cam[ocam[1]].W2I(OptPosition_cam1[tid]);
				for (int i2 = 0; i2 < finder[ocam[1]].x.size(); i2++) {
					if (!(OptPosition_cam2[tid].x - (eps2 / root) < finder[ocam[1]].x[i2] && finder[ocam[1]].x[i2] < OptPosition_cam2[tid].x + (eps2 / root) &&
						OptPosition_cam2[tid].y - (eps2 / root) < finder[ocam[1]].y[i2] && finder[ocam[1]].y[i2] < OptPosition_cam2[tid].y + (eps2 / root))) continue;
#pragma endregion
					a2[tid] = OptPosition_cam2[tid].x - finder[ocam[1]].x[i2];
					b2[tid] = OptPosition_cam2[tid].y - finder[ocam[1]].y[i2];

#pragma region Display secondCam
					Mat secondCam = imread(conf.ImagePath[ocam[1]][fcnt]);
					if (display) {
						rectangle(secondCam, Rect(OptPosition_cam2[tid].x - (eps2 / root), OptPosition_cam2[tid].y - (eps2 / root), eps2 * root, eps2 * root), Scalar(255, 0, 0), 1, 1);
						circle(secondCam, Point2f(finder[ocam[1]].x[i2], finder[ocam[1]].y[i2]), 2, Scalar(0, 255, 0), 1);
						circle(secondCam, Point2f(OptPosition_cam2[tid].x, OptPosition_cam2[tid].y), 2, Scalar(0, 0, 255), 1);
						imshow("Second Cam", secondCam);
						moveWindow("Second Cam", 0, 1040);
						waitKey(0);
					}
#pragma endregion

					dist2[tid] = sqrt(a2[tid] * a2[tid] + b2[tid] * b2[tid]);
					if (dist2[tid] > eps2) continue;
                    
					OptPosition_cam3[tid] = cam[ocam[2]].W2I(OptPosition_cam1[tid]);
					for (int i3 = 0; i3 < finder[ocam[2]].x.size(); i3++) {
						if (!(OptPosition_cam3[tid].x - (eps2 / root) < finder[ocam[2]].x[i3] && finder[ocam[2]].x[i3] < OptPosition_cam3[tid].x + (eps2 / root) &&
							OptPosition_cam3[tid].y - (eps2 / root) < finder[ocam[2]].y[i3] && finder[ocam[2]].y[i3] < OptPosition_cam3[tid].y + (eps2 / root))) continue;

						a3[tid] = OptPosition_cam3[tid].x - finder[ocam[2]].x[i3];
						b3[tid] = OptPosition_cam3[tid].y - finder[ocam[2]].y[i3];

#pragma region Display thirdCam
						if (display) {
							Mat thirdCam = imread(conf.ImagePath[ocam[2]][fcnt]);
							rectangle(thirdCam, Rect(OptPosition_cam3[tid].x - eps2 / root, OptPosition_cam3[tid].y - eps2 / root, eps2 * root, eps2 * root), Scalar(255, 0, 0), 1, 1);
							circle(thirdCam, Point2f(finder[ocam[2]].x[i3], finder[ocam[2]].y[i3]), 2, Scalar(0, 255, 0), 1);
							circle(thirdCam, Point2f(OptPosition_cam3[tid].x, OptPosition_cam3[tid].y), 2, Scalar(0, 0, 255), 1);
							imshow("Third Cam", thirdCam);
							moveWindow("Third Cam", 1030, 1040);
							waitKey(0);
						}
#pragma endregion

						dist3[tid] = sqrt(a3[tid] * a3[tid] + b3[tid] * b3[tid]);
						if (dist3[tid] > eps2) continue;



						candTmp[tid].xref = finder[rcam].x[ir], candTmp[tid].yref = finder[rcam].y[ir];
						candTmp[tid].x1 = finder[ocam[0]].x[i1], candTmp[tid].y1 = finder[ocam[0]].y[i1];
						candTmp[tid].x2 = finder[ocam[1]].x[i2], candTmp[tid].y2 = finder[ocam[1]].y[i2];
						candTmp[tid].x3 = finder[ocam[2]].x[i3], candTmp[tid].y3 = finder[ocam[2]].y[i3];

						Traigulation2cam(finder[rcam], ir, finder[ocam[0]], i1, cam[rcam], cam[ocam[0]], FinalOpt1[tid]);
						Traigulation2cam(finder[rcam], ir, finder[ocam[1]], i2, cam[rcam], cam[ocam[1]], FinalOpt2[tid]);
						Traigulation2cam(finder[rcam], ir, finder[ocam[2]], i3, cam[rcam], cam[ocam[2]], FinalOpt3[tid]);
						toGetFinal[tid] = Position(finder[rcam].x[ir], finder[rcam].y[ir], 0);
						FinalOpt1[tid] = cam[rcam].I2W(toGetFinal[tid], FinalOpt1[tid].z);
						FinalOpt2[tid] = cam[rcam].I2W(toGetFinal[tid], FinalOpt2[tid].z);
						FinalOpt3[tid] = cam[rcam].I2W(toGetFinal[tid], FinalOpt3[tid].z);

						candTmp[tid].Xr1 = FinalOpt1[tid].x; candTmp[tid].Xr2 = FinalOpt2[tid].x; candTmp[tid].Xr3 = FinalOpt3[tid].x;
						candTmp[tid].Yr1 = FinalOpt1[tid].y; candTmp[tid].Yr2 = FinalOpt2[tid].y; candTmp[tid].Yr3 = FinalOpt3[tid].y;
						candTmp[tid].Zr1 = FinalOpt1[tid].z; candTmp[tid].Zr2 = FinalOpt2[tid].z; candTmp[tid].Zr3 = FinalOpt3[tid].z;

						if (RangeTestPr(1) && RangeTestPr(2) && RangeTestPr(3))
							tmpWorldPosition[tid].push_back(candTmp[tid]);

					}
				}
			}
		}
#pragma endregion


#pragma region Get Final Position
		for (int trecnt = 0; trecnt < ThreadNum; trecnt++)
			for (int cnt = 0; cnt < tmpWorldPosition[trecnt].size(); cnt++) mPs.push_back(tmpWorldPosition[trecnt][cnt]);
		cout << "\b ", cout.fill(' '), cout.width(6), cout << mPs.size() << " |";
#pragma endregion


#pragma region Tecplot Write Particle position
		string conffilename = conf.ProjFolder + "Particle\\Frame" + to_string(fcnt + conf.S_FrameNum) + ".plt";
		_mkdir((conf.ProjFolder + "Particle").c_str());
		string out_line;
		ofstream outfile;
		outfile.open(conffilename, ios::trunc);
		outfile << "TITLE = \"Particle Time : " << to_string(fcnt + conf.S_FrameNum) << "\"" << endl;
		outfile << "VARIABLES = \"x\" \"y\" \"z\" \"I\"" << endl;
		outfile << "ZONE T= \"" << to_string(fcnt + conf.S_FrameNum) << "\"" << endl;
		outfile << " I =" << to_string(mPs.size()) << ", J=1, K=1, F=POINT" << endl;
		for (int i = 0; i < mPs.size(); i++)
			outfile << to_string(mPs[i].Xr1) << "\t" << to_string(mPs[i].Yr1) << "\t" << to_string(mPs[i].Zr1) << "\t100" << endl;
		outfile.close();
#pragma endregion
#pragma region Writing results
		string Matched_Path = conf.ProjFolder + "Matched";
		_mkdir(Matched_Path.c_str());
		Matched_Path = Matched_Path + "\\" + to_string(rcam + 1); _mkdir(Matched_Path.c_str());
		Matched_Path = Matched_Path + "\\Frame" + to_string(fcnt + conf.S_FrameNum) + ".dat";
		WritePosition_B(Matched_Path, mPs);
#pragma endregion
#pragma region Clear Variables for next iter
		for (int trecnt = 0; trecnt < ThreadNum; trecnt++) tmpWorldPosition[trecnt].clear();
		finder.clear();
		cands.clear();
		mPs.clear();
#pragma endregion
		time_t e = clock(); 
		cout.fill(' '); cout.width(6); cout.precision(2);
		cout << (double)(e - s) / 1000 << "s |" << endl;
	}
}
#endif // !VSC_MATCHING_H
