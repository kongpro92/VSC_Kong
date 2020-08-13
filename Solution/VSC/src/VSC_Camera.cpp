#include <VSC_Camera.h>
#include <string.h>
#include <limits.h>

#define P(j,i) P.at<double>(##j,##i)
#define U(j,i) U.at<double>(##j,##i)
#define T(j,i) tvecs[0].at<double>(##j,##i)
#define R(j,i) R.at<double>(##j,##i)

using namespace std;

Camera::Camera(istream& is)
{
    
}

Camera::Camera(string path)
{
    ifstream in(path, ios::in);
    string line;
    if (in.is_open()) {
        getline(in, line); line.erase(line.find_first_of(' '));
        width = atoi(line.c_str());

        getline(in, line); line.erase(line.find_first_of(' '));
        height = atoi(line.c_str());

       
        for (int i = 0; i < 12; ++i) {
            getline(in, line); line.erase(line.find_first_of(' '));
            buffer12[i] = atof(line.c_str());
        }
        P = Mat(3,4,CV_64FC1);
        for (int j = 0;j<3;j++)for (int i = 0;i<4;i++)P.at<double>(j, i) = buffer12[i+j*4];
    }
}

 Position Camera::W2I( Position p) 
{
     buffer4[0] = p.x, buffer4[1] = p.y, buffer4[2] = p.z, buffer4[3] = 1;
     Mat point = Mat(4,1,CV_64FC1,buffer4);
	 Mat U = P*point; U/= U(2, 0);
     return Position(U(0,0),U(1,0),0);
}

 Position Camera::I2W( Position& p,double z) 
 {
    u = p.x,v=p.y;
    l[0] = P(0, 0) - u* P(2, 0), l[1] = P(0, 1) - u* P(2, 1);
    l[2] = P(1, 0) - v* P(2, 0), l[3] = P(1, 1) - v* P(2, 1);
    Mat lmat = Mat(2,2,CV_64FC1,l);
    r[0] = z*(u*P(2, 2) - P(0, 2))+ u*P(2, 3) - P(0, 3);
    r[1] = z*(v*P(2, 2) - P(1, 2))+ v*P(2, 3) - P(1, 3);
	Mat rmat = Mat(2, 1, CV_64FC1, r);
	Mat Pmat = lmat.inv()*rmat;

    return Position(Pmat.at<double>(0,0),Pmat.at<double>(1,0),z);
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
     for (int j = 0;j<3;j++)for (int i = 0;i<3;i++)P.at<double>(j, i) = R_.at<double>(j, i);
     P(0, 3) = tvecs[0].at<double>(0, 0);
     P(1, 3) = tvecs[0].at<double>(1, 0);
     P(2, 3) = tvecs[0].at<double>(2, 0);

     P = cameraMat * P;

     std::cout << "Camera Matrix" << std::endl;
     std::cout << cameraMat << std::endl;
     std::cout << std::endl;
 }

 void Camera::WriteParameter(string path)
 {
     ofstream outfile;
     outfile.open(path, ios::trunc);
     outfile << "1024 //Image Width" << endl;
     outfile << "1024 //Image Height" << endl;
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

     //Camera 내부파리미터
     for (int i = 0;i < originals.size();i++)if (originals[i][2] == 0)
         originalsz0.push_back(originals[i]), centresz0.push_back(centres[i]);
     if(originalsz0.size()==0) cout<<"Z = 0 No data"    <<endl;
     OBJECTSZ0.push_back(originalsz0), IMAGESZ0.push_back(centresz0);
     cameraMat = initCameraMatrix2D(OBJECTSZ0, IMAGESZ0, ImgSize);


     OBJECTS.push_back(originals), IMAGES.push_back(centres);
     double d[] = { 0,0,0,0 };
     Mat distCoeffs(4, 1, CV_64FC1, d);
     calibrateCamera(OBJECTS, IMAGES, ImgSize, cameraMat, distCoeffs, rvecs, tvecs, stdDeviationsIntrinsics, stdDeviationsExtrinsics, perViewErrors, CV_CALIB_USE_INTRINSIC_GUESS);
    
    
     Mat R = Mat::zeros(3, 3, CV_64FC1); Rodrigues(rvecs[0], R);
     for (int j = 0;j<3;j++)for (int i = 0;i<3;i++)P(j, i) = R(j, i);
                              for(int i =0;i<3;i++)P(i, 3) = T(i, 0);

     P = cameraMat * P;


     cout << "Camera Matrix" << std::endl;
     cout << cameraMat << std::endl;
     cout<< "Discoefficent" << endl << distCoeffs;
     cout << std::endl;

     WriteParameter(path);
 }