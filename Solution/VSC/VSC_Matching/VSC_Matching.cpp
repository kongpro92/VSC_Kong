#include "VSC_Matching.h"

int main(int argc, char *argv[])
{
	SetConsole("VSC_Matching...");
	conf.ReadConfigurationFile(argv[1]);
	if (conf.ProjFolder == "") return 0;

	SetReferenceCamera(rcam, ocam);
	SetCamParams();
	SetEpsilon(eps1, eps2);
    SetDisplay(display);
	VSC_Matching();

	return 0;
}