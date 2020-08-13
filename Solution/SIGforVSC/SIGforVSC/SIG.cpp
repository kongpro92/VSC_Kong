#include "SIG.h"



int main(int argc, char** argv)
{
	readconfiguration(string(argv[1]), bd, camcnt, cam, pcnt, psize,isImportFF,FFfolderpath,isSteadyFlow,imgnum);
	par = Particle(pcnt, psize);

	block = (Block(Boundary(-100, -75, -2, -50, 0, 2)));

	par.MakeRandomPosition(bd, block);
	
	namelist = new vector<string>[camcnt];

	for(int cnt =0;cnt< imgnum;cnt++){
		cout << cnt << endl;
		img.clear();
		for (int camcnt = 0; camcnt < par_in_img.size(); camcnt++)par_in_img[camcnt].clear();
		par_in_img.clear();

		par.Project2Image(cam);
		for (int camcnt = 0; camcnt < cam.size(); camcnt++)img.push_back(MakeImage(par, cam, par_in_img,camcnt)); waitKey(1);

		SaveImage(string(argv[2]), img, par_in_img, namelist, cnt);
		SetFlowField(ff, isImportFF, FFfolderpath, isSteadyFlow);
		
		//ff.ApplyFlowField(bd, block, par);

	}
	SaveNameList(string(argv[2]), namelist);

	return 0;
}