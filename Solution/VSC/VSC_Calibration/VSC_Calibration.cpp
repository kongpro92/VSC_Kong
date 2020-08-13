#define _CRT_SECURE_NO_WARNINGS
#include "VSC_Calibration.h""




void main(int argc, char* argv[])
{
    SetConsole("VSC_Calibration");
    cout<<endl<<"\tWelcome to VSC Calibration"<<endl<<endl;    
    ReadInitFile(argv[1]);
    SetSubVoxleNode();
    SetReferenceCamera(rcam,ocam);
    SetValidateProperty();

    cout << "\tRead Calibration Parameter " << ReadCameraParameter()    << endl;
    cout << "\tRead Matched Points "        << ReadAllMatchedPoint_B()  << endl;
    cout << "\tSet Disparitymap";              Dismap = Disparitymap(conf.Camcnt, conf.Xnum, conf.Ynum, conf.Znum,conf.eps);
    cout << "\tMake Disparity 2D map"       << MakeDisparity2D()<<endl;
    cout << "\tFind Disparity 2D map Peak"  << FindDisparity2DPeak()<<endl;
    cout << "\tValidation Disparity 3D map" << Dismap.Validation(nb,thr, medianeps,Gsigam);
    cout << "\tWrite Disparity 3D map"      << WriteDisparity3Dmap();
    cout << "\treCalibrate Camera Parameter"<< ReCalibration();
}