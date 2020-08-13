#ifndef SIG_CONFIG_H
#define SIG_CONFIG_H
#include <fstream>
#include <string>
#include <sstream>
#include <direct.h>
#include <time.h>
#include <deque>

#include "../include/boundary.h"
#include "../include/Camera.h"

using namespace std;

bool saveconfiguration(string path, Boundary &bd, int &camcnt, deque<Camera> &cam, int &pcnt, 
					   double&psize, bool isImportFF, string &FFfolderpath, bool isSteadyFlow, int imgnum);
template <typename T> string to_string_(const T a_value, const int n = 6);
string timenow();
bool readconfiguration(string path, Boundary &bd, int &camcnt, deque<Camera> &cam, int &pcnt,
					   double&psize, bool &isImportFF, string &FFfolderpath, bool &isSteadyFlow, int &imgnum);

#endif // !SIG_CONFIG_H



