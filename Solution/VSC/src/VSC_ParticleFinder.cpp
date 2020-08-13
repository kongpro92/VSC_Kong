#include <VSC_ParticleFinder.h>
#include <VSC_Logs.h>
#include <iostream>
#include <fstream>

using namespace std;

void ParticleFinder::FindParticle2DCenter(int depth, int threshold) {
    int colors = depth;

    // walk through the given array, skipping the first and last row and column
    for (int i = 1; i < (rows - 1); i++) {
        for (int j = 1; j < (cols - 1); j++) {
            // is this pixel a local maximum above threshold?
            if ((pixels[i][j] >= threshold) && IsLocalMax(i, j)) {
                // read in the local maximum pixel value as well as the values to its
                // left and right and top and bottom in order to calculate the
                // particle center.  note: add 0.5 to row and column values to put
                // the pixel origin in its center
                double x1 = (j - 1);// +0.5;
                double x2 = j;// +0.5;
                double x3 = (j + 1);// +0.5;
                double y1 = (i - 1);// +0.5;
                double y2 = i;// + 0.5;
                double y3 = (i + 1);// +0.5;

                                    // check the pixel values to make sure we have no corrupted memory
                if ((abs(pixels[i][j]) > colors) ||
                    (abs(pixels[i - 1][j]) > colors) ||
                    (abs(pixels[i + 1][j]) > colors) ||
                    (abs(pixels[i][j - 1]) > colors) ||
                    (abs(pixels[i][j + 1]) > colors)) {
           
                }

                // find the column value, moving 0 intensities to 0.0001 so we can
                // take their log
                double lnz1, lnz2, lnz3;
                if (colors == 255) {
                    if (pixels[i][j - 1] == 0) {
                        lnz1 = log(0.0001);
                    }
                    else {
                        lnz1 = Logs::log8bit[pixels[i][j - 1]];
                    }
                    if (pixels[i][j] == 0) {
                        lnz2 = log(0.0001);
                    }
                    else {
                        lnz2 = Logs::log8bit[pixels[i][j]];
                    }
                    if (pixels[i][j + 1] == 0) {
                        lnz3 = log(0.0001);
                    }
                    else {
                        lnz3 = Logs::log8bit[pixels[i][j + 1]];
                    }
                }
                else if (colors == 65535) {
                    lnz1 = Logs::log16bit[pixels[i][j - 1]];
                    lnz2 = Logs::log16bit[pixels[i][j]];
                    lnz3 = Logs::log16bit[pixels[i][j + 1]];
                }
                else {
                    if (pixels[i][j - 1] == 0) {
                        lnz1 = log(0.0001);
                    }
                    else {
                        lnz1 = log(static_cast<double>(pixels[i][j - 1]));
                    }
                    if (pixels[i][j] == 0) {
                        lnz2 = log(0.0001);
                    }
                    else {
                        lnz2 = log(static_cast<double>(pixels[i][j]));
                    }
                    if (pixels[i][j + 1] == 0) {
                        lnz3 = log(0.0001);
                    }
                    else {
                        lnz3 = log(static_cast<double>(pixels[i][j + 1]));
                    }
                }

                double xc = -0.5 * ((lnz1 * ((x2 * x2) - (x3 * x3))) - (lnz2 * ((x1 * x1) - (x3 * x3))) + (lnz3 * ((x1 * x1) - (x2 * x2)))) / ((lnz1 * (x3 - x2)) - (lnz3 * (x1 - x2)) + (lnz2 * (x1 - x3)));

                // were these numbers valid?
                if (!isfinite(xc)) {
                    // no -- we had a problem.  drop this particle
                    continue;
                }

                // find the row value
                if (colors == 255) {
                    if (pixels[i - 1][j] == 0) {
                        lnz1 = log(0.0001);
                    }
                    else {
                        lnz1 = Logs::log8bit[pixels[i - 1][j]];
                    }
                    if (pixels[i + 1][j] == 0) {
                        lnz3 = log(0.0001);
                    }
                    else {
                        lnz3 = Logs::log8bit[pixels[i + 1][j]];
                    }
                }
                else if (colors == 65535) {
                    lnz1 = Logs::log16bit[pixels[i - 1][j]];
                    lnz3 = Logs::log16bit[pixels[i + 1][j]];
                }
                else {
                    if (pixels[i - 1][j] == 0) {
                        lnz1 = log(0.0001);
                    }
                    else {
                        lnz1 = log(static_cast<double>(pixels[i - 1][j]));
                    }
                    if (pixels[i + 1][j] == 0) {
                        lnz3 = log(0.0001);
                    }
                    else {
                        lnz3 = log(static_cast<double>(pixels[i + 1][j]));
                    }
                }

                double yc = -0.5 * ((lnz1 * ((y2 * y2) - (y3 * y3))) - (lnz2 * ((y1 * y1) - (y3 * y3))) +
                    (lnz3 * ((y1 * y1) - (y2 * y2))))
                    / ((lnz1 * (y3 - y2)) - (lnz3 * (y1 - y2)) + (lnz2 * (y1 - y3)));

                // check these numbers too
                if (!isfinite(yc)) {
                    // a problem occurred, so we'll drop these numbers and move on
                    continue;
                }
                //cout <<y.size() +1<<"  " <<i<<","<<j<<"  "<< "xc: " << xc << " yc: " << yc << endl;
                y.push_back(yc);
                x.push_back(xc);
                inten.push_back(pixels[i][j]);
            }
        }
    }
}

bool ParticleFinder::IsLocalMax(int r, int c)
{
    int val = pixels[r][c];

    // check only the 4 neighbors directly above, below, to the left, and to the right
    if ((pixels[r][c - 1] > val) || (pixels[r][c + 1] > val) || (pixels[r - 1][c] > val) || (pixels[r + 1][c] > val)||
        (pixels[r-1][c - 1] > val) || (pixels[r-1][c + 1] > val) || (pixels[r + 1][c-1] > val) || (pixels[r + 1][c-1] > val)) {
        return false;
    }

    return true;
}

void ParticleFinder::readParticle2DPosition(string path) {
	ifstream in(path);
	string line;
	int num = 0; 
	getline(in, line);
	num = atoi(line.c_str());

	for (int i = 0; i < num; i++) {
		getline(in, line);	x.push_back(atof(line.c_str()));
		getline(in, line);	y.push_back(atof(line.c_str()));
		inten.push_back(100);
	}
	in.close();	
}