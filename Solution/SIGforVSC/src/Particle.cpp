#include "../include/Particle.h"

Particle::Particle()
{
}
Particle::Particle(int num_, double size_) {
	num = num_, size = size_;

}
Particle::~Particle()
{
}

void Particle::MakeRandomPosition(Boundary bd, Block bl)
{
	random_device rdx,rdy,rdz;
	mt19937 genx(rdx()), geny(rdy()), genz(rdz());
	uniform_real_distribution<double> disx(bd.minx, bd.maxx);
	uniform_real_distribution<double> disy(bd.miny, bd.maxy);
	uniform_real_distribution<double> disz(bd.minz, bd.maxz);

	for (int i = 0; i < num; i++) {
		pos3d.push_back(Point3d(disx(genx), disy(geny), disz(genz)));
		cout << "Particle Making : " << i + 1 << "/" << num <<endl;
		for (int j = 0; j < i; j++) {
			if(bl.bd.minx <= pos3d[i].x &&pos3d[i].x <= bl.bd.maxx &&
				bl.bd.miny <= pos3d[i].y &&pos3d[i].y <= bl.bd.maxy &&
				bl.bd.minz <= pos3d[i].z &&pos3d[i].z <= bl.bd.maxz)
			{
				pos3d.pop_back(); i--;
				break;
			}
			 Point3d difpos = pos3d[i] - pos3d[j];
			 if (sqrt(difpos.dot(difpos)) < size) {
				 pos3d.pop_back(); i--;
				 break;
			 }
		}
	}
}

void Particle::MoveRandomPositionYZ(Boundary bd, Block bl, int idx) {
	random_device rdx, rdy, rdz;
	mt19937 genx(rdx()), geny(rdy()), genz(rdz());
	uniform_real_distribution<double> disx(0, bd.maxx*2/100);
	uniform_real_distribution<double> disy(bd.miny, bd.maxy);
	uniform_real_distribution<double> disz(bd.minz, bd.maxz);

	if (bd.maxx < pos3d[idx].x)pos3d[idx].x -= bd.maxx*2;
	if (pos3d[idx].x < bd.minx)pos3d[idx].x += bd.maxx*2;
	pos3d[idx].y = disy(geny);
	pos3d[idx].z = disz(genz);

}

void Particle::Project2Image(deque<Camera> cam) {
	pos2d.clear();

	for (int camcnt = 0; camcnt < cam.size(); camcnt++) {
		vector<Point2d> tmppos2d;
		projectPoints(pos3d, cam[camcnt].rvec, cam[camcnt].tvec, cam[camcnt].cameramatrix, cam[camcnt].distCoeffs, tmppos2d);
		pos2d.push_back(tmppos2d);
	}
	

}