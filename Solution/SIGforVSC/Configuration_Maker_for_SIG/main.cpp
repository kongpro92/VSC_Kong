#include "main.h"

int main(int argc, char** argv) {
    saveconfiguration(argload(argc, argv),bd,camcnt,cam,pcnt,psize,isImportFF,FFfolderpath,isSteadyFlow,imgnum);
    return 0;
}

