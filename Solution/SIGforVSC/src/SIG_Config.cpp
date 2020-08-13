#include "..//include/SIG_Config.h"


bool saveconfiguration(string path, Boundary &bd, int &camcnt, deque<Camera> &cam, int &pcnt, 
					   double&psize, bool isImportFF, string &FFfolderpath, bool isSteadyFlow, int imgnum) {
	cout << path << endl;;


	string filename = timenow();
	string filepath = path + timenow() + ".config";
	ofstream out(filepath);
	if (out.is_open()) {
		out << bd.maxx * 2 << " //Volume Size X(mm)\n";
		out << bd.maxy * 2 << " //Volume Size Y(mm)\n";
		out << bd.maxz * 2 << " //Volume Size Z(mm)\n";
		out << camcnt << " //Number of Camera\n";
		for (int cnt = 0; cnt < camcnt; cnt++) {
			out << cam[cnt].width   << " //Camera " << to_string(cnt) << " width\n";
			out << cam[cnt].height  << " //Camera " << to_string(cnt) << " height\n";
			out << cam[cnt].Getfx() << " //Camera " << to_string(cnt) << " fx\n";
			out << cam[cnt].Getfy() << " //Camera " << to_string(cnt) << " fy\n";
			out << cam[cnt].Getcx() << " //Camera " << to_string(cnt) << " cx\n";
			out << cam[cnt].Getcy() << " //Camera " << to_string(cnt) << " cy\n";
			out << cam[cnt].GetAlpha() << " //Camera " << to_string(cnt) << " Rotation Alpha(degree)\n";
			out << cam[cnt].GetBeta()  << " //Camera " << to_string(cnt) << " Rotation  Beta(degree)\n";
			out << cam[cnt].GetGamma() << " //Camera " << to_string(cnt) << " Rotation Gamma(degree)\n";
			out << cam[cnt].GetTx() << " //Camera " << to_string(cnt) << " Translation Tx(mm)\n";
			out << cam[cnt].GetTy() << " //Camera " << to_string(cnt) << " Translation Ty(mm)\n";
			out << cam[cnt].GetTz() << " //Camera " << to_string(cnt) << " Translation Tz(mm)\n";
			for (int idx = 0; idx < 14; idx++) {
				string tmpstr(" Distortion Coefficient ");
				switch (idx)
				{
				case 0: tmpstr += "k1\n"; break;
				case 1: tmpstr += "k2\n"; break;
				case 2: tmpstr += "p1\n"; break;
				case 3: tmpstr += "p2\n"; break;
				case 4: tmpstr += "k3\n"; break;
				case 5: tmpstr += "k4\n"; break;
				case 6: tmpstr += "k5\n"; break;
				case 7: tmpstr += "k6\n"; break;
				case 8: tmpstr += "s1\n"; break;
				case 9: tmpstr += "s2\n"; break;
				case 10: tmpstr += "s3\n"; break;
				case 11: tmpstr += "s4\n"; break;
				case 12: tmpstr += "tau_x\n"; break;
				case 13: tmpstr += "tau_y\n"; break;
				}
				out << cam[cnt].GetDist(idx) << " //Camera " << to_string(cnt) << tmpstr;
			}
			out << cam[cnt].GetParSize() << " //Camera " << to_string(cnt) << " Particle Size in Image(mm)\n";
		}
		out << pcnt << " Number of Particle\n";
		out << psize << " Size of Particle(mm)\n";
		out << isImportFF << " is Import Flow Field?\n";
		out << FFfolderpath << " Flow Field folder path\n";
		out << isSteadyFlow << " is Steady Flow?\n";
		out << imgnum << " number of image\n";
	}
	out.close();
	return true;
}

template <typename T>
string to_string_(const T a_value, const int n)
{
	ostringstream out;
	out.precision(n);
	out << fixed << a_value;
	return out.str();
}

string timenow()
{
	time_t curTime = time(NULL);
	struct tm tm;
	localtime_s(&tm, &curTime);
	string year, month, day, hour, min, sec;
	int monthi = tm.tm_mon + 1;
	int dayi = tm.tm_mday;
	int houri = tm.tm_hour;
	int mini = tm.tm_min;
	int seci = tm.tm_sec;

	year = to_string(tm.tm_year - 100);
	if (monthi < 10)month = "0" + to_string(monthi); else month = to_string(monthi);
	if (dayi < 10)day = "0" + to_string(dayi);   else day = to_string(dayi);
	if (houri < 10)hour = "0" + to_string(houri);  else hour = to_string(houri);
	if (mini < 10)min = "0" + to_string(mini);   else min = to_string(mini);
	if (seci < 10)sec = "0" + to_string(seci);   else sec = to_string(seci);

	string res = year + month + day + "_" + hour + min + sec;
	return res;
}

bool readconfiguration(string path, Boundary &bd, int &camcnt, deque<Camera> &cam, int &pcnt,
					   double&psize, bool &isImportFF, string &FFfolderpath, bool &isSteadyFlow, int &imgnum)
{
	ifstream in(path);
	string tmpstr;
	double tmpf; int tmpi;
	if (in.is_open()) {
		getline(in, tmpstr, ' '); tmpf = stod(tmpstr); bd.minx = -tmpf / 2.0; bd.maxx = tmpf / 2.0; getline(in, tmpstr);
		getline(in, tmpstr, ' '); tmpf = stod(tmpstr); bd.miny = -tmpf / 2.0; bd.maxy = tmpf / 2.0; getline(in, tmpstr);
		getline(in, tmpstr, ' '); tmpf = stod(tmpstr); bd.minz = -tmpf / 2.0; bd.maxz = tmpf / 2.0; getline(in, tmpstr);
		getline(in, tmpstr, ' '); tmpi = stoi(tmpstr); camcnt = tmpi; getline(in, tmpstr);
		for (int cnt = 0; cnt < camcnt; cnt++) {
			int w, h;
			double fx, fy, cx, cy, rx, ry, rz, tx, ty, tz, k1, k2, p1, p2, k3, k4, k5, k6, s1, s2, s3, s4, tau_x, tau_y, psize;
			getline(in, tmpstr, ' '); w = stoi(tmpstr); 	getline(in, tmpstr);
			getline(in, tmpstr, ' '); h = stoi(tmpstr);		getline(in, tmpstr);
			getline(in, tmpstr, ' '); fx = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); fy = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); cx = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); cy = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); rx = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); ry = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); rz = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); tx = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); ty = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); tz = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); k1 = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); k2 = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); p1 = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); p2 = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); k3 = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); k4 = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); k5 = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); k6 = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); s1 = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); s2 = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); s3 = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); s4 = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); tau_x = stod(tmpstr);	getline(in, tmpstr);
			getline(in, tmpstr, ' '); tau_y = stod(tmpstr); getline(in, tmpstr);
			getline(in, tmpstr, ' '); psize = stod(tmpstr); getline(in, tmpstr);
			cam.push_back(
				*new Camera(w, h, fx, fy, cx, cy, rx, ry, rz, tx, ty, tz, k1, k2, p1, p2, k3, k4, k5, k6, s1, s2, s3, s4, tau_x, tau_y, psize)
			);
		}
		getline(in, tmpstr, ' '); pcnt = stoi(tmpstr); getline(in, tmpstr);
		getline(in, tmpstr, ' '); psize = stod(tmpstr); getline(in, tmpstr);
		getline(in, tmpstr, ' '); isImportFF = stoi(tmpstr); getline(in, tmpstr);
		getline(in, tmpstr, ' '); FFfolderpath = tmpstr; getline(in, tmpstr);
		getline(in, tmpstr, ' '); isSteadyFlow = stoi(tmpstr); getline(in, tmpstr);
		getline(in, tmpstr, ' '); imgnum = stoi(tmpstr); getline(in, tmpstr);
		return true;
	}

}