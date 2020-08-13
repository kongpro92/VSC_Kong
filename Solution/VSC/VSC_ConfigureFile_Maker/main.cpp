#include "VSC_Configuration.h"

Configuration conf;

void main() {
	conf = Configuration(string("D:\\Kong\\VSC_Project_(20.08.11~20.09.18)\\Data\\Test"),4,0,100,-20,20,-20,20,-10,10);
	conf.MakeConfigurationFile();
}