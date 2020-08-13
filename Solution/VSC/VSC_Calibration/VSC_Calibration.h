#pragma once

#include "..\include\VSC_Configuration.h"
#include "..\include\VSC_Common.h"
#include "..\include\VSC_Camera.h"
#include "..\include\VSC_Disparitymap.h"
#include <iostream>
#include <fstream>
#include <deque>
#include <direct.h>

#define mPsX(ocam)  mPs[pcnt].Xr##ocam
#define mPsY(ocam)  mPs[pcnt].Yr##ocam
#define mPsZ(ocam)  mPs[pcnt].Zr##ocam
#define mPsx(ocam)  mPs[pcnt].x##ocam
#define mPsy(ocam)  mPs[pcnt].y##ocam

using namespace std;

Configuration conf;
int rcam;               //referecne camera number
deque<int> ocam;        //other cameras number
deque<Position> mPs;    //Matched Point
deque<Camera> cam;
deque<double> nodeX, nodeY, nodeZ;
Disparitymap Dismap;
float eps;
double Xgap,Ygap,Zgap;
int Xnum,Ynum,Znum;
int ix, iy, iz, Idx;

//for 2D Disparity Map  
Position pw, pi;            //pw : World to Image position, pi : Image position
float dx, dy;               //dx : disparity x, dy = disparity y
int imgX, imgY;         //imgX, imgY : float to int for image index
double ex, ey, sx, sy;      //ex,ey,sx,sy : for linear interpolation
float r;                    //r : distance between float and int index
float val;                  //val : result of gaussian

//for Find Disparity Peak
float **vecx, **vecy;

//for Validate Disparity 3D map
int nb;                 //Filter Size 1 -> 3x3x3, 2 -> 5x5x5
double thr;             //Median Threshold value
double medianeps;       //Median Epsilon
double Gsigam;          //Gaussian Smoothing sigma 

void ReadInitFile(string path)
{
    conf.ReadConfigurationFile(path);
    conf.ReadVSCConditionFile();
    Xgap = conf.Xgap, Ygap = conf.Ygap, Zgap = conf.Zgap;
    Xnum = conf.Xnum, Ynum = conf.Ynum, Znum = conf.Znum;
    eps = conf.eps;
}
void SetSubVoxleNode()
{
    for (int i = 0;i < conf.Xnum;i++)nodeX.push_back(conf.Xmin + i * conf.Xgap);
    for (int i = 0;i < conf.Ynum;i++)nodeY.push_back(conf.Ymin + i * conf.Ygap);
    for (int i = 0;i < conf.Znum;i++)nodeZ.push_back(conf.Zmin + i * conf.Zgap);
}
string GetMatchedFilePath(int rcam, int fcnt)
{
    string tmp;
    tmp = conf.ProjFolder + "Matched\\" + to_string(rcam+1)+"\\Frame"+to_string(fcnt + conf.S_FrameNum) + ".dat";
    return tmp;
}
string ReadCameraParameter()
{
    for(int i=0;i<4;i++)cam.push_back(Camera(conf.CalibrationFilePath[i]));
    return "Done";
}
void ReadMatchedPoint_B(string path, deque<Position>&mps)
{
    int Pcnt = 0;
    ifstream in(path, ios::in | ios::binary);
    if (in.is_open()) {
        in.read((char*)&Pcnt, sizeof(int));
        Position tmpp;
        for (int pcnt = 0;pcnt < Pcnt;pcnt++) {
            in.read((char*)&tmpp.xref, sizeof(double));
            in.read((char*)&tmpp.yref, sizeof(double));
            in.read((char*)&tmpp.x1, sizeof(double));
            in.read((char*)&tmpp.y1, sizeof(double));
            in.read((char*)&tmpp.x2, sizeof(double));
            in.read((char*)&tmpp.y2, sizeof(double));
            in.read((char*)&tmpp.x3, sizeof(double));
            in.read((char*)&tmpp.y3, sizeof(double));
            in.read((char*)&tmpp.Xr1, sizeof(double));
            in.read((char*)&tmpp.Yr1, sizeof(double));
            in.read((char*)&tmpp.Zr1, sizeof(double));
            in.read((char*)&tmpp.Xr2, sizeof(double));
            in.read((char*)&tmpp.Yr2, sizeof(double));
            in.read((char*)&tmpp.Zr2, sizeof(double));
            in.read((char*)&tmpp.Xr3, sizeof(double));
            in.read((char*)&tmpp.Yr3, sizeof(double));
            in.read((char*)&tmpp.Zr3, sizeof(double));
            mPs.push_back(tmpp);
        }
        in.close();
    }
    else {
        cout << "\tThere is no file - " << path << endl;
    }
}
string ReadAllMatchedPoint_B()
{
    for (int fcnt = 0;fcnt < conf.Framecnt;fcnt++) ReadMatchedPoint_B(GetMatchedFilePath(rcam, fcnt), mPs);
    return "- " + to_string(mPs.size());
}
string MakeDisparity2D()
{
    for (int pcnt = 0;pcnt < mPs.size();pcnt++) {
        for (int cnt = 0; cnt < 3;cnt++) {
            ix = 999, iy = 999, iz = 999;		//index x,y,z in disparity volume
                 if (cnt == 0) { pw.x = mPsX(1), pw.x = mPsY(1), pw.x = mPsZ(1); }
            else if (cnt == 1) { pw.x = mPsX(2), pw.x = mPsY(2), pw.x = mPsZ(2); }
            else if (cnt == 2) { pw.x = mPsX(3), pw.x = mPsY(3), pw.x = mPsZ(3); }
            for (int i = 0;i < nodeX.size();i++)if (nodeX[i] - Xgap / 2 <= pw.x && pw.x < nodeX[i] + Xgap / 2)ix = i;   //Find Particle location index in subvolume;
            for (int j = 0;j < nodeY.size();j++)if (nodeY[j] - Ygap / 2 <= pw.y && pw.y < nodeY[j] + Ygap / 2)iy = j;   //Find Particle location index in subvolume;
            for (int k = 0;k < nodeZ.size();k++)if (nodeZ[k] - Zgap / 2 <= pw.z && pw.z < nodeZ[k] + Zgap / 2)iz = k;   //Find Particle location index in subvolume;
            if (ix != 999 && iy != 999 && iz != 999)Idx = ix + iy * Xnum + iz * Xnum*Ynum;                              //3D index to 1D index(subvolume);
            pw = cam[ocam[cnt]].W2I(Position(pw.x, pw.y, pw.z));                                                        //perspective traigulation position World to Image

                 if (cnt == 0) { pi.x = mPsx(1), pi.y = mPsy(1); }
            else if (cnt == 1) { pi.x = mPsx(2), pi.y = mPsy(2); }
            else if (cnt == 2) { pi.x = mPsx(3), pi.y = mPsy(3); }

            if (0 <= Idx && Idx < Xnum*Ynum*Znum) {
                dx = pw.x - pi.x, dy = pw.y - pi.y;
                if (-eps <= dx && dx <= eps&&-eps <= dy && dy <= eps) {
                    dx = (dx + eps) * 10, dy = (dy + eps) * 10;
                    imgX = (int)round(dx), imgY = (int)round(dy);
                    if (imgX - 1 >= 0 && imgX + 1 < Dismap.size2d && imgY - 1 >= 0 && imgY + 1 < Dismap.size2d) {
                        for (int j = -1;j <= 1;j++)for (int i = -1;i <= 1;i++) {
                            r = (i + imgX - dx)*(i + imgX - dx) + (j + imgY - dy)*(j + imgY - dy);
                            val = exp(-r / 0.49);
                            Dismap.disparity[cnt][Idx][i + imgX + (j + imgY) * Dismap.size2d] += val;
                        }
                    }
                }
            }
        }
    }
    vecx = new float*[3], vecy = new float*[3];
    for (int cnt = 0;cnt < 3;cnt++) {
        vecx[cnt] = new float[Xnum*Ynum*Znum];
        vecy[cnt] = new float[Xnum*Ynum*Znum];
    }

    return " Done";
}
string FindDisparity2DPeak()
{
    for (int camcnt = 0;camcnt < 3;camcnt++) {
        for (int k = 0;k < Znum;k++) for (int j = 0;j < Ynum;j++)for (int i = 0;i < Xnum;i++) {
            int volidx = i + j*Xnum + k*Xnum*Ynum; float max = -1; int maxi, maxj;
            for (int jj = 0;jj < Dismap.size2d;jj++)for (int ii = 0;ii < Dismap.size2d;ii++) {
                int imgidx = ii + jj* Dismap.size2d;
                if (max < Dismap.disparity[camcnt][volidx][imgidx]) {
                    max = Dismap.disparity[camcnt][volidx][imgidx];
                    maxi = ii, maxj = jj;
                }
            }
            int peakidx = maxi + maxj*Dismap.size2d;
            vecx[camcnt][volidx] = maxi + subpixel(Dismap.disparity[camcnt][volidx][peakidx - 1], 
                                                   Dismap.disparity[camcnt][volidx][peakidx + 0], 
                                                   Dismap.disparity[camcnt][volidx][peakidx + 1]);
            vecy[camcnt][volidx] = maxj + subpixel(Dismap.disparity[camcnt][volidx][peakidx - Dismap.size2d],
                                                   Dismap.disparity[camcnt][volidx][peakidx],
                                                   Dismap.disparity[camcnt][volidx][peakidx + Dismap.size2d]);
            if (max != 0) {
                for (int jj = 0;jj < Dismap.size2d;jj++)for (int ii = 0;ii < Dismap.size2d;ii++){
                    int imgidx = ii + Dismap.size2d*jj;
                    Dismap.disparity[camcnt][volidx][imgidx] *= (255/(float)max);
                }
            }
        }
    }
    return "Done";
}
void SetValidateProperty()
{
    cout <<"\tSet Disparity 3D map Validation Property"<<endl;
    cout <<"\tFilter Size          ( 1 ) : "; cin>>nb;
    cout <<"\tThreshold Value      (0.1) : "; cin>>thr;
    cout <<"\tMedian Filter Epsilon(0.1) : "; cin>>medianeps;
    cout <<"\tGaussian Filter sigma(0.5) : "; cin>>Gsigam; cout <<endl;
}

string WriteDisparity3Dmap()
{   
    string path = conf.ProjFolder +"Disparity"; _mkdir(path.c_str());
    path = path+"\\3Dmap";_mkdir(path.c_str());
    path = path + "\\Ref Camera" + to_string(rcam+1); _mkdir(path.c_str());
    path = path + "\\";
    Dismap.Write3DMap(path,rcam,ocam,nodeX,nodeY,nodeZ);
    return " Done";
}
string ReCalibration()
{
    vector<Vec3f>originals; vector<Vec2f>centres;
    for (int ccnt = 0; ccnt < 3;ccnt++){
        for (int k = 0;k < Znum;k++)for (int j = 0;j < Ynum;j++)for (int i = 0;i < Xnum;i++) {
            Vec3f point3d(nodeX[i], nodeY[j], nodeZ[k]);

            Position imgp = cam[ocam[ccnt]].W2I(Position(nodeX[i], nodeY[j], nodeZ[k]));
            imgp.x -= Dismap.vox[ccnt][k][j][i].x; imgp.y -= Dismap.vox[ccnt][k][j][i].y;
            Vec2f point2d = Vec2f(imgp.x, imgp.y);

            originals.push_back(point3d);
            centres.push_back(point2d);
        }
        string savepath;
        savepath = conf.ProjFolder + "VSC_Parameter" + to_string(ocam[ccnt] + 1) + ".txt";
        cam[ccnt].CalibrateCamera_(originals, centres, Size(cam[ocam[ccnt]].width, cam[ocam[ccnt]].height), savepath);
        originals.clear(); centres.clear();
    }
    return " Done";
}