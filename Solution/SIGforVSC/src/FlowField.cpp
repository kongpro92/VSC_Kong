#include "../include/FlowField.h"


FlowField::FlowField()
{
}

FlowField::~FlowField()
{
}

void FlowField::ApplyFlowField(Boundary bd, Block bl, Particle &par) {
	double searchsize = par.size*10;
	for (int pcnt = 0; pcnt < par.num; pcnt++) {
		deque<int> near;
		deque<double> difnear;
		while (near.size() < 4 || near.size() > 20) {
			near.clear(); difnear.clear();
			for (int i = 0; i < pt.size(); i++) {
				Point3d dif = pt[i] - par.pos3d[pcnt];
				if (sqrt(dif.dot(dif)) < searchsize) {
					near.push_back(i);
					difnear.push_back(dif.dot(dif));
				}
			}
			cout << searchsize << "  " << near.size() << endl;
			if (near.size() < 4) searchsize *= 1.2;
			if (near.size() > 20) searchsize /= 1.2;
		}
		//보간부 적어두기
		Point3d tmpvelo(0, 0, 0);
		double tmpd=0;
		for (int i = 0; i < near.size(); i++) {
			tmpvelo += velo[near[i]] / difnear[i];
			tmpd += 1 / difnear[i];
		}
		tmpvelo /= tmpd;

		//이동거리 추가
		par.pos3d[pcnt] += tmpvelo;
		if (bd.maxx < par.pos3d[pcnt].x || par.pos3d[pcnt].x < bd.minx) {
			//par.MoveRandomPositionYZ(bd, bl,pcnt);
		}
	}
}

void FlowField::importffdata(string path) {
	pt.clear(); velo.clear();
	//deque<Point3d> pt_; 속도를 위해 위치 분할이 필요함
	//deque<Point3d> velo_;

	ifstream in(path);
	string tmpstr;
	stringstream ss(tmpstr);
	deque<string> data;
	if (in.is_open()) {
		getline(in, tmpstr); getline(in, tmpstr); getline(in, tmpstr); getline(in, tmpstr); getline(in, tmpstr); getline(in, tmpstr);
		while (getline(in, tmpstr)) {
			data.clear();
			ss = stringstream(tmpstr);
			while (getline(ss, tmpstr, ',')) {
				data.push_back(tmpstr);
			}
			pt.push_back(Point3d(stod(data[0]) *1000, stod(data[1]) *1000, stod(data[2]) * 1000));
			velo.push_back(Point3d(stod(data[3]) * 1000, stod(data[4]) * 1000, stod(data[5]) * 1000));
		}
	}

	//auto it = max_element(std::begin(pt_), std::end(pt_));
}
