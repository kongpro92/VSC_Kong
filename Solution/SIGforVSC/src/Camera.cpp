#include "../include/Camera.h"


Camera::Camera()
{
}

Camera::Camera(int w, int h, double fx, double fy, double cx, double cy, double alpha, double beta, double gamma, double t1, double t2, double t3,
    double dist1, double dist2, double dist3, double dist4, double dist5, double dist6, double dist7, double dist8, double dist9,
    double dist10, double dist11, double dist12, double dist13, double dist14, double psize) {
	width = w, height = h;
    cmdata[0] = fx, cmdata[4] = fy, cmdata[2] = cx, cmdata[5] = cy; cmdata[8] = 1;
    rdata[0] = alpha, rdata[1] = beta, rdata[2] = gamma;
    tdata[0] = t1, tdata[1] = t2, tdata[2] = t3;
    distdata[0] = dist1; distdata[1] = dist2; distdata[2] = dist3; distdata[3] = dist4;
    distdata[4] = dist5; distdata[5] = dist6; distdata[6] = dist7; distdata[7] = dist8;
    distdata[8] = dist9; distdata[9] = dist10; distdata[10] = dist11; distdata[11] = dist12;
    distdata[12] = dist13; distdata[13] = dist14; 
	parsize = psize;
	
    cameramatrix = Mat(3, 3, CV_64F, &cmdata);
    Rmat2Rvec();
    tvec = Mat(1,3,CV_64F,&tdata);
    distCoeffs = Mat(1, 14, CV_64F, &distdata);
	P = Mat(3, 4, CV_64FC1);
}
Camera::~Camera()
{
}

void Camera::InsertParameter() {
    SetImgSize();
    Setfocal();
    SetRotation();
    SetTranslation();
    SetDistcoeffs();
    cout << endl;
}
#pragma region InsertParameter
void Camera::SetImgSize(){
    cout << "  width : ";cin >> width; cout << "  height : ";cin >> height;
}
void Camera::Setfocal() {
    *cmdata = { 0 };
    cout << "  fx : ";cin >> cmdata[0]; cout << "  fy : ";cin >> cmdata[4];
    cout << "  cx : ";cin >> cmdata[2]; cout << "  cy : ";cin >> cmdata[5];
    cmdata[8] = 1;
    cameramatrix = Mat(3, 3, CV_64F, &cmdata);
}

void Camera::SetRotation() {
    *rdata = { 0 };
    cout << "  α : ";cin >> rdata[0];  cout << "  β : ";cin >> rdata[1];  cout << "  γ : ";cin >> rdata[2];
    Rmat2Rvec();
}

void Camera::SetTranslation() {
    *tdata = { 0 };
    cout << "  X : ";cin >> tdata[0];  cout << "  Y : ";cin >> tdata[1];  cout << "  Z : ";cin >> tdata[2];
    tvec = Mat(1, 3, CV_64F, &tdata);

}

void Camera::SetDistcoeffs() {
    *distdata = { 0 };
    int menu;
    cout << "  distortion is none (Yes : 1 / No : 0)"; cin >> menu;
    if (menu != 1)for (int i = 0;i < 14;i++) {
        cout << "  " << to_string(i); cin >> distdata[i];
    }
    distCoeffs = Mat(1, 14, CV_64F, &distdata);
}
#pragma endregion


string Camera::strImageSize()
{
    return string("      ├  Image Size              ─  width : " + to_string(width) + "  height : " + to_string(height) + "\n");
}

string Camera::strCameraMatrix() {
    string res;
    res =  "      ├  Camera Matrix           ┬  fx : " + to_string(cameramatrix.at<double>(0, 0)) + "  fy : " + to_string(cameramatrix.at<double>(1, 1)) + "\n";
    res += "      │                          └  cx : " + to_string(cameramatrix.at<double>(0, 2)) + "  cy : " + to_string(cameramatrix.at<double>(1, 2)) + "\n";;

    return res;
}

string Camera::strRotationAngle() {
    return string("      ├  Rotation Anglex         ─  α : " + to_string(rdata[0]) + "  β : " + to_string(rdata[1]) + "  γ: " + to_string(rdata[2]) + "\n");
}

string Camera::strTranslation(){
    return string("      ├  Translation             ─  X : " + to_string(tdata[0]) + "  Y : " + to_string(tdata[1]) + "  Z: " + to_string(tdata[2]) + "\n");
}

string Camera::strDistcoeffs() {
    string res;
    res =  "      └  Distortion Coefficients ┬  k1 : " + to_string(distdata[ 0]) + "  k2 : " + to_string(distdata[ 1]) + "  p1: " + to_string(distdata[ 2]) + "  p2: " + to_string(distdata[ 3]) + "\n";
    res += "                                 │  k3 : " + to_string(distdata[ 4]) + "  k4 : " + to_string(distdata[ 5]) + "  k5: " + to_string(distdata[ 6]) + "  k6: " + to_string(distdata[ 7]) + "\n";
    res += "                                 │  s1 : " + to_string(distdata[ 8]) + "  s2 : " + to_string(distdata[ 9]) + "  s3: " + to_string(distdata[10]) + "  s4: " + to_string(distdata[11]) + "\n";
    res += "                                 └  τx : " + to_string(distdata[12]) + "  τy : " + to_string(distdata[13]) + "\n";
    return res;
}

void Camera::Rmat2Rvec() {
    double x = rdata[0] * CV_PI / 180.0, y = rdata[1] * CV_PI / 180.0, z = rdata[2] * CV_PI / 180.0;
    Mat Rx = Mat::zeros(3, 3, CV_64F);Mat Ry = Mat::zeros(3, 3, CV_64F);Mat Rz = Mat::zeros(3, 3, CV_64F);
    Rx.at<double>(0, 0) = 1, Rx.at<double>(1, 1) = cos(x), Rx.at<double>(1, 2) = -sin(x);
    Rx.at<double>(2, 1) = sin(x), Rx.at<double>(2, 2) = cos(x);
    Ry.at<double>(0, 0) = cos(y), Ry.at<double>(0, 2) = sin(y), Ry.at<double>(1, 1) = 1;
    Ry.at<double>(2, 0) = -sin(y), Ry.at<double>(2, 2) = cos(y);
    Rz.at<double>(0, 0) = cos(z), Rz.at<double>(0, 1) = -sin(z), Rz.at<double>(1, 0) = sin(z);
    Rz.at<double>(1, 1) = cos(z), Rz.at<double>(2, 2) = 1;
    Mat R = Rz * Ry*Rx;
    Rodrigues(R, rvec);
}

void Camera::CalibrateCamera_(vector<Vec3f>originals, vector<Vec2f>centres, Size ImgSize)
{
	vector<vector<Vec2f>> IMAGES;     vector<vector<Vec3f>> OBJECTS;
	Mat cameraMat(3, 3, CV_64FC1);
	Mat 	stdDeviationsIntrinsics;
	Mat 	stdDeviationsExtrinsics;
	Mat 	perViewErrors;
	vector<Mat> rvecs, tvecs;

	double d[] = { 0,0,0,0 };
	Mat distCoeffs(4, 1, CV_64FC1, d);

	cv::calibrateCamera(OBJECTS, IMAGES, ImgSize, cameraMat, distCoeffs, rvecs, tvecs, stdDeviationsIntrinsics, stdDeviationsExtrinsics, perViewErrors, CV_CALIB_USE_INTRINSIC_GUESS);
	cv::Mat R_ = Mat::zeros(3, 3, CV_64FC1);
	cv::Rodrigues(rvecs[0], R_);
	for (int j = 0; j < 3; j++)for (int i = 0; i < 3; i++)P.at<double>(j, i) = R_.at<double>(j, i);
	P(0, 3) = tvecs[0].at<double>(0, 0);
	P(1, 3) = tvecs[0].at<double>(1, 0);
	P(2, 3) = tvecs[0].at<double>(2, 0);

	P = cameraMat * P;

	std::cout << "Camera Matrix" << std::endl;
	std::cout << cameraMat << std::endl;
	std::cout << std::endl;
}

void Camera::CalibrateCamera_(vector<Vec3f>originals, vector<Vec2f>centres, Size ImgSize, string path)
{
	vector<vector<Vec2f>> IMAGES;     vector<vector<Vec3f>> OBJECTS;
	vector<vector<Vec2f>> IMAGESZ0;     vector<vector<Vec3f>> OBJECTSZ0;
	vector<Vec2f>centresz0; vector<Vec3f> originalsz0;

	Mat cameraMat(3, 3, CV_64FC1);
	Mat 	stdDeviationsIntrinsics;
	Mat 	stdDeviationsExtrinsics;
	Mat 	perViewErrors;
	vector<Mat> rvecs, tvecs;

	for (int i = 0; i < originals.size(); i++)if (originals[i][2] == 0)
		originalsz0.push_back(originals[i]), centresz0.push_back(centres[i]);
	if (originalsz0.size() == 0) cout << "Z = 0 No data" << endl;
	OBJECTSZ0.push_back(originalsz0), IMAGESZ0.push_back(centresz0);
	cameraMat = initCameraMatrix2D(OBJECTSZ0, IMAGESZ0, ImgSize);


	OBJECTS.push_back(originals), IMAGES.push_back(centres);
	double d[] = { 0,0,0,0 };
	Mat distCoeffs(4, 1, CV_64FC1, d);
	calibrateCamera(OBJECTS, IMAGES, ImgSize, cameraMat, distCoeffs, rvecs, tvecs, stdDeviationsIntrinsics, stdDeviationsExtrinsics, perViewErrors, CV_CALIB_USE_INTRINSIC_GUESS);


	Mat R = Mat::zeros(3, 3, CV_64FC1); Rodrigues(rvecs[0], R);
	for (int j = 0; j < 3; j++)for (int i = 0; i < 3; i++)P(j, i) = R(j, i);
	for (int i = 0; i < 3; i++)P(i, 3) = T(i, 0);

	P = cameraMat * P;


	/*cout << "Camera Matrix" << std::endl;
	cout << cameraMat << std::endl;
	cout << "Discoefficent" << endl << distCoeffs;
	cout << std::endl;*/
	/*cout << "ViewErrors" << endl;
	cout << perViewErrors << endl << endl;*/
	WriteParameter(path);
}

void Camera::WriteParameter(string path)
{
	ofstream outfile;
	outfile.open(path, ios::trunc);
	outfile << width << " //Image Width" << endl;
	outfile << height << " //Image Height" << endl;
	outfile << to_string(P.at<double>(0, 0)) << " //P(0,0)" << endl;
	outfile << to_string(P.at<double>(0, 1)) << " //P(0,1)" << endl;
	outfile << to_string(P.at<double>(0, 2)) << " //P(0,2)" << endl;
	outfile << to_string(P.at<double>(0, 3)) << " //P(0,3)" << endl;
	outfile << to_string(P.at<double>(1, 0)) << " //P(1,0)" << endl;
	outfile << to_string(P.at<double>(1, 1)) << " //P(1,1)" << endl;
	outfile << to_string(P.at<double>(1, 2)) << " //P(1,2)" << endl;
	outfile << to_string(P.at<double>(1, 3)) << " //P(1,3)" << endl;
	outfile << to_string(P.at<double>(2, 0)) << " //P(2,0)" << endl;
	outfile << to_string(P.at<double>(2, 1)) << " //P(2,1)" << endl;
	outfile << to_string(P.at<double>(2, 2)) << " //P(2,2)" << endl;
	outfile << to_string(P.at<double>(2, 3)) << " //P(2,3)" << endl;
	outfile.close();
}