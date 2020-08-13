#include <VSC_Common.h>
#include <VSC_Camera.h>
#include <VSC_ParticleFinder.h>

void SetDisplay(bool &display)
{
    cout << "\tShow Display(1:YES || 0:NO) : "; cin >> display;
}

void SetConsole(string title)
{
    string tmp = "title " + title;
    system(tmp.c_str()); system("mode con lines=60 cols=150");
}

void SetEpsilon(float &eps1_, float &eps2_)
{
	// eps1 for equation for line
	// eps2 for square
	cout << "\tInsert First  Epsilon Value : "; cin >> eps1_;
	cout << "\tInsert Second Epsilon Value : "; cin >> eps2_;
}

void SetReferenceCamera(int &rcam_, deque<int> &ocam_)
{
    cout << endl << "\tInsert Reference Camera Number : "; cin >> rcam_;
    for(int i=0;i<4;i++)if(i!= rcam_ -1)ocam_.push_back(i);
    cout<<          "\tOther Cameras Number           : ";
    for (int i = 0;i < ocam_.size();i++)cout << ocam_[i] + 1 << ", "; cout << "\b\b " << endl<<endl; rcam_--;
}

void CopyMatToArr(Mat mat, int ** dst)
{
	for (int j = 0; j < mat.rows; j++) for (int i = 0; i < mat.cols; i++) dst[j][i] = mat.at<uchar>(j, i);
}

int** CreateArr(int height, int width)
{
	int **pixels = new int *[height];
	for (int i = 0; i < height; i++) pixels[i] = new int[width];

	return pixels;
}

void WritePosition_B(string path, deque<Position> &p)
{
	fstream out(path, ios::out | ios::binary);

	int size = p.size();
	out.write((char*)&size, sizeof(int));
	for (int i = 0; i < size; i++) {
		out.write((char *)&(p[i].xref), sizeof(double));
		out.write((char *)&(p[i].yref), sizeof(double));
		out.write((char *)&(p[i].x1), sizeof(double));
		out.write((char *)&(p[i].y1), sizeof(double));
		out.write((char *)&(p[i].x2), sizeof(double));
		out.write((char *)&(p[i].y2), sizeof(double));
		out.write((char *)&(p[i].x3), sizeof(double));
		out.write((char *)&(p[i].y3), sizeof(double));
		out.write((char *)&(p[i].Xr1), sizeof(double));
		out.write((char *)&(p[i].Yr1), sizeof(double));
		out.write((char *)&(p[i].Zr1), sizeof(double));
		out.write((char *)&(p[i].Xr2), sizeof(double));
		out.write((char *)&(p[i].Yr2), sizeof(double));
		out.write((char *)&(p[i].Zr2), sizeof(double));
		out.write((char *)&(p[i].Xr3), sizeof(double));
		out.write((char *)&(p[i].Yr3), sizeof(double));
		out.write((char *)&(p[i].Zr3), sizeof(double));
	}
	out.close();
}

void WritePosition(string path, deque<Position> &p)
{
	fstream out(path, ios::out);

	int size = p.size();
	out << size << "\n";
	for (int i = 0; i < size; i++) {
		out << p[i].xref << " ";
		out << p[i].yref << " ";
		out << p[i].x1 << " ";
		out << p[i].y1 << " ";
		out << p[i].x2 << " ";
		out << p[i].y2 << " ";
		out << p[i].x3 << " ";
		out << p[i].y3 << " ";
		out << p[i].Xr1 << " ";
		out << p[i].Yr1 << " ";
		out << p[i].Xr2 << " ";
		out << p[i].Yr2 << " ";
		out << p[i].Xr3 << " ";
		out << p[i].Yr3 << "\n";
	}
	out.close();
}

void Traigulation2cam(ParticleFinder finder1, int idx1, ParticleFinder finder2, int idx2, Camera cam1, Camera cam2, Position &dst)
{
	Mat opti_p1(4, 4, CV_64FC1);

	for (int i = 0; i < 4; i++) {
		opti_p1.at<double>(0, i) = finder1.x[idx1] * cam1.P.at<double>(2, i) - cam1.P.at<double>(0, i);
		opti_p1.at<double>(1, i) = finder1.y[idx1] * cam1.P.at<double>(2, i) - cam1.P.at<double>(1, i);
		opti_p1.at<double>(2, i) = finder2.x[idx2] * cam2.P.at<double>(2, i) - cam2.P.at<double>(0, i);
		opti_p1.at<double>(3, i) = finder2.y[idx2] * cam2.P.at<double>(2, i) - cam2.P.at<double>(1, i);
	}
	Mat w, u, v;
	SVDecomp(opti_p1, w, u, v);

	double x = v.at<double>(3, 0) / v.at<double>(3, 3);
	double y = v.at<double>(3, 1) / v.at<double>(3, 3);
	double z = v.at<double>(3, 2) / v.at<double>(3, 3);

	dst.x = x;     dst.y = y;     dst.z = z;
}

void Traigulation4cam(Position candi, Camera cam1, Camera cam2, Camera cam3, Camera cam4, Position &dst)
{
	//최적합 지점 구하기
	Mat optimum_point(8, 4, CV_64FC1);
	for (int i = 0; i < 4; i++) {
		optimum_point.at<double>(0, i) = candi.xref * cam1.P.at<double>(2, i) - cam1.P.at<double>(0, i);
		optimum_point.at<double>(1, i) = candi.yref * cam1.P.at<double>(2, i) - cam1.P.at<double>(1, i);
		optimum_point.at<double>(2, i) = candi.x1 * cam2.P.at<double>(2, i) - cam2.P.at<double>(0, i);
		optimum_point.at<double>(3, i) = candi.y1 * cam2.P.at<double>(2, i) - cam2.P.at<double>(1, i);
		optimum_point.at<double>(4, i) = candi.x2 * cam3.P.at<double>(2, i) - cam3.P.at<double>(0, i);
		optimum_point.at<double>(5, i) = candi.y2 * cam3.P.at<double>(2, i) - cam3.P.at<double>(1, i);
		optimum_point.at<double>(6, i) = candi.x3 * cam4.P.at<double>(2, i) - cam4.P.at<double>(0, i);
		optimum_point.at<double>(7, i) = candi.y3 * cam4.P.at<double>(2, i) - cam4.P.at<double>(1, i);
	}
	Mat w, u, v;
	SVDecomp(optimum_point, w, u, v);

	double x = v.at<double>(3, 0) / v.at<double>(3, 3);
	double y = v.at<double>(3, 1) / v.at<double>(3, 3);
	double z = v.at<double>(3, 2) / v.at<double>(3, 3);

	dst.x = x; dst.y = y; dst.z = z;
	dst.xref = candi.xref, dst.yref = candi.yref;
	dst.x1 = candi.x1, dst.y1 = candi.y1;
	dst.x2 = candi.x2, dst.y2 = candi.y2;
	dst.x3 = candi.x3, dst.y3 = candi.y3;
}
float subpixel(float a1, float a2, float a3)
{
    float l1, l2, l3;
    if (a1 < 0.0001) l1 = log(0.0001);else l1 = log(a1);
    if (a2 < 0.0001) l2 = log(0.0001);else l2 = log(a2);
    if (a3 < 0.0001) l3 = log(0.0001);else l3 = log(a3);

    float mo = 2 * a1 - 4 * a2 + 2 * a3;
    if (abs(mo) < 0.0001)mo = 0.0001;
    float ja = a1 - a3;
    float res = ja / mo;
    if (res < -0.5) res = -0.5; else if (res > 0.5)res = 0.5;
    return res;
}
