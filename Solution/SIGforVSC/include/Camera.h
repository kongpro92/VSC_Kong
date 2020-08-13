#ifndef Camera_H
#define Camera_H

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>

#define P(j,i) P.at<double>(##j,##i)
#define U(j,i) U.at<double>(##j,##i)
#define T(j,i) tvecs[0].at<double>(##j,##i)
#define R(j,i) R.at<double>(##j,##i)

using namespace cv;
using namespace std;

class Camera
{
public:
	Camera();
	Camera(int w, int h, double fx, double fy, double cx, double cy, double alpha, double beta, double gamma, double t1, double t2, double t3,
		double dist1, double dist2, double dist3, double dist4, double dist5, double dist6, double dist7, double dist8, double dist9,
		double dist10, double dist11, double dist12, double dist13, double dist14, double psize);
	~Camera();

	void InsertParameter();
	void SetImgSize();
	void Setfocal();
	void SetRotation();
	void SetTranslation();
	void SetDistcoeffs();
	string strImageSize();
	string strCameraMatrix();
	string strRotationAngle();
	string strTranslation();
	string strDistcoeffs();
	void Rmat2Rvec();

	string Getfx() { return to_string(cmdata[0]); };
	string Getfy() { return to_string(cmdata[4]); };
	string Getcx() { return to_string(cmdata[2]); };
	string Getcy() { return to_string(cmdata[5]); };
	string GetAlpha() { return to_string(rdata[0]); };
	string GetBeta() { return to_string(rdata[1]); };
	string GetGamma() { return to_string(rdata[2]); };
	string GetTx() { return to_string(tdata[0]); };
	string GetTy() { return to_string(tdata[1]); };
	string GetTz() { return to_string(tdata[2]); };
	string GetDist(int idx) { return to_string(distdata[idx]); };
	string GetParSize() { return to_string(parsize); };


	int width = 1024, height = 1024;
	Mat cameramatrix;
	Mat rvec;
	Mat tvec;
	Mat distCoeffs;
	Mat P;

	double parsize; //particle size for SIG

	void CalibrateCamera_(vector<Vec3f>originals, vector<Vec2f>centres, Size ImgSize);
	void CalibrateCamera_(vector<Vec3f>originals, vector<Vec2f>centres, Size ImgSize, string path);
	void WriteParameter(string path);

private:
	double cmdata[9] = { 0 };
	double rdata[3] = { 0 };
	double tdata[3] = { 0 };
	double distdata[14] = { 0 };
	// cameramatrix
	// rvec
	// tvec
	// distCoeffs
};


#endif // !Camera_H


