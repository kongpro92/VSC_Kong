#pragma once


#ifndef VSC_COFIGURATION_H
#define VSC_COFIGURATION_H

#pragma once
#include <deque>
#include <string>
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <io.h>
#include <time.h>

using namespace std;
class Configuration {
public:
    Configuration();
	Configuration(string path,int camcnt,int sfn,int efn, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax);
    ~Configuration() {};


public:
    string ProjFolder;                          //Project Folder path
    int Camcnt;                                 //Cameara Number        
    int S_FrameNum;                             //Start Frame Number
    int E_FrameNum;                             //End   Frame Number
    int Framecnt;                               //Number of Frames
    deque<string> ImageNameListFile;            //Each Image Name File of Cameras
    deque<deque<string>> ImagePath;             //Each Image's paths of Cameras
    deque<string> CalibrationFilePath;          //Each Camera Calibratino file path 
    double Xmin, Xmax, Ymin, Ymax, Zmin, Zmax;  //Volume Size for Measurement
    double Xgap, Ygap, Zgap;                    //3D Disparity Grid Gap size
    int Xnum, Ynum, Znum;                       //3D Disparity Grid Number
    float eps;                                   //epsilon for VSC

public:
    void MakeConfigurationFile();
    void ReadConfigurationFile(string path);
    void ReadVSCConditionFile();
    void ShowConfiguration();
    void ShowVSCCondition();
};
#endif // !VSC_COFIGURATION_H
