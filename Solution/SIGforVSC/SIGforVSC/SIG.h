#include "../include/SIG_Config.h"
#include "../include/Particle.h"
#include "../include/FlowField.h"
#include "../include/Blocking.h"

Boundary bd;
Block block;
int camcnt;
deque<Camera> cam;
int pcnt;
double psize;
Particle par;
deque<Mat> img;
deque<deque<Point2d>> par_in_img;
bool isImportFF; // Is Import Flow Field
string FFfolderpath; // Flow Field 데이터 폴더 경로
bool isSteadyFlow;
int imgnum; // 제작할 이미지 개수
FlowField ff;

vector<string> *namelist;

bool FlowField::ffimport = (true);
bool FlowField::Steady = (true);


#define LASERINTEN 100
#define SIGMA 1
#define ParPos par.pos2d[idx][cnt]
#define Gaussian2D exp(-1 * (difpos.x*difpos.x + difpos.y*difpos.y) / (2 * sigma*sigma))

Mat MakeImage(Particle par, deque<Camera> cam, deque<deque<Point2d>> &parinimg, int idx);
Mat MakeImage(Particle par, Particle par_, deque<Camera> cam, int idx);
void SaveImage(string path, deque<Mat> img, deque<deque<Point2d>> par_in_img, vector<string>* namelist, int idx);
void SaveNameList(string path, vector<string> *namelist);

Mat MakeImage(Particle par, deque<Camera> cam, deque<deque<Point2d>> &parinimg, int idx)
{
	deque<Point2d> parinimg_;
	Mat mat = Mat::zeros(cam[idx].height, cam[idx].width, CV_8UC1);
	int maxpsize = (int)ceil(cam[idx].parsize / 2.0);
	for (int cnt = 0; cnt < par.num; cnt++) {
		double sigma = cam[idx].parsize / (3.92); //95% Sigma
		if (0 < ParPos.x && ParPos.x < cam[idx].width && 0 < ParPos.y && ParPos.y < cam[idx].height) { // Is Particle Position in Image?
			parinimg_.push_back(ParPos);
			for (int j = -maxpsize; j <= maxpsize; j++)for (int i = -maxpsize; i <= maxpsize; i++) {
				Point2d pos(round(ParPos.x) + i, round(ParPos.y) + j);
				Point2d difpos = ParPos - pos;
				double dis = sqrt(difpos.dot(difpos)); // Distance
				if (dis < cam[idx].parsize / 2.0) {
					if (0 < pos.x && pos.x < cam[idx].width && 0 < pos.y && pos.y < cam[idx].height) {  // Is Particle Drawing Position in Image?
						int tmp = mat.at<uchar>(pos.y, pos.x) + (LASERINTEN * Gaussian2D);
						if (tmp > 255) tmp = 255;
						mat.at<uchar>(pos.y, pos.x) = (uchar)tmp;
					}
				}
			}
		}
	}
	parinimg.push_back(parinimg_);
	
	imshow(to_string(idx), mat);
	
	return mat;
}


Mat MakeImage(Particle par, Particle par_, deque<Camera> cam, int idx)
{

	Mat mat = Mat::zeros(cam[idx].height, cam[idx].width, CV_8UC1);
	int maxpsize = (int)ceil(cam[idx].parsize / 2.0);
	for (int cnt = 0; cnt < par.num; cnt++) {
		double sigma = cam[idx].parsize / (3.92); //95% Sigma
		if (0 < ParPos.x && ParPos.x < cam[idx].width && 0 < ParPos.y && ParPos.y < cam[idx].height) { // Is Particle Position in Image?
			for (int j = -maxpsize; j <= maxpsize; j++)for (int i = -maxpsize; i <= maxpsize; i++) {
				Point2d pos(round(ParPos.x) + i, round(ParPos.y) + j);
				Point2d difpos = ParPos - pos;
				double dis = sqrt(difpos.dot(difpos)); // Distance
				if (dis < cam[idx].parsize / 2.0) {
					if (0 < pos.x && pos.x < cam[idx].width && 0 < pos.y && pos.y < cam[idx].height) {  // Is Particle Drawing Position in Image?
						int tmp = mat.at<uchar>(pos.y, pos.x) + (LASERINTEN * Gaussian2D);
						if (tmp > 255) tmp = 255;
						mat.at<uchar>(pos.y, pos.x) = (uchar)tmp;
					}
				}
			}
		}
	}
	rectangle(mat, par_.pos2d[0][0], par_.pos2d[0][1], Scalar(255, 255, 255));
	imshow("d", mat);
	waitKey(1);
	return mat;
}

void SetFlowField(FlowField &ff, bool isImportFF, string &FFfolderpath, bool isSteadyFlow) {
	
	double movx = 0.5;
	double movy = 0;
	double movz = 0;

	/// 범위밖으로 벗어난 입자를 다시랜덤위치로 복귀를 위해 선언
	random_device rdx, rdy, rdz;
	mt19937 genx(rdx()), geny(rdy()), genz(rdz());
	uniform_real_distribution<double> disx(-movx*0.6, movx*0.6);
	//uniform_real_distribution<double> disy(bd.miny, bd.maxy);
	//uniform_real_distribution<double> disz(bd.minz, bd.maxz);

	uniform_real_distribution<double> r_bdy(bd.miny, bd.maxy);
	uniform_real_distribution<double> r_bdz(bd.minz, bd.maxz);


	if (isImportFF) {
		if (isSteadyFlow) {
			if (ff.pt.size() != 0)return;
			FlowField::ffimport = true;  FlowField::Steady = true;

			ff.importffdata(FFfolderpath + "\\export.csv");

		}
		else {
		}
	}
	else {
		if (isSteadyFlow) {
			for (int i = 0; i < par.num; i++) {
				par.pos3d[i].x += 0.5;
				if (par.pos3d[i].x > bd.maxx) {
					par.pos3d[i].x = bd.minx + disx(genx);
					par.pos3d[i].y = r_bdy(geny);
				}
			}
			
		}
	}
}

void SaveImage(string path, deque<Mat> img, deque<deque<Point2d>> par_in_img, vector<string>* namelist, int idx) {
	for (int camcnt = 0; camcnt < cam.size(); camcnt++) {
		string dirpath = path + "\\Cam" + to_string(camcnt);
		_mkdir(dirpath.c_str());
		string imgpath = dirpath + "\\" + to_string(idx + 100000) + ".tiff";
		imwrite(imgpath ,img[camcnt]);
		namelist[camcnt].push_back("Cam" + to_string(camcnt)+ "\\" + to_string(idx + 100000) + ".tiff");

		string posfilepath = path + "\\Cam" + to_string(camcnt) + "_Position";
		_mkdir(posfilepath.c_str());

		ofstream out(posfilepath + "\\" + to_string(idx + 100000) + ".dat");
		if (out.is_open()) {
			out << par_in_img[camcnt].size() << endl;
			for (int parcnt = 0; parcnt < par_in_img[camcnt].size(); parcnt++) {
				out << par_in_img[camcnt][parcnt].x << endl;
				out << par_in_img[camcnt][parcnt].y << endl;
			}
			out.close();
		}

	}
}

void SaveNameList(string path, vector<string> *namelist){
	for (int cnt = 0; cnt < camcnt; cnt++) {
		string Path = path + "cam" + to_string(cnt) + "ImageNames.txt";
		ofstream out(Path);
		if (out.is_open()) {
			for (int namecnt = 0; namecnt < namelist[cnt].size(); namecnt++) {
				out << namelist[cnt][namecnt] << endl;
			}
			out.close();
		}
	}
}