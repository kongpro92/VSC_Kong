#include <VSC_Disparitymap.h>


Disparitymap::Disparitymap()
{
}

Disparitymap::Disparitymap(int camnum, int vxnum, int vynum, int vznum, float eps)
{
    Camnum = camnum - 1;
    xnum =vxnum, ynum = vynum,znum = vznum;
    //Disparity 계산
    vox = new Point2f***[Camnum]; errvox = new bool***[Camnum];
#pragma omp parallel for
    for (int cnt = 0;cnt < Camnum;cnt++) {
        vox[cnt] = new Point2f**[znum]; errvox[cnt] = new bool**[znum];
        for (int k = 0;k < znum;k++) {
            vox[cnt][k] = new Point2f*[ynum]; errvox[cnt][k] = new bool*[ynum];
            for (int j = 0;j<ynum;j++) {
                vox[cnt][k][j] = new Point2f[xnum]; errvox[cnt][k][j] = new bool[xnum];
            }
        }
    }

    tmpvox = new Point2f**[znum];
#pragma omp parallel for
    for (int k = 0;k < znum;k++) {
        tmpvox[k] = new Point2f*[ynum];
        for (int j = 0;j < ynum;j++)tmpvox[k][j] = new Point2f[xnum];
    }

    deque<float* > tmpdisparity;
    size2d = eps * 2 * 10 + 1;
    for (int cnt = 0;cnt < 4;cnt++) {
        for (int k = 0;k < znum + 1;k++) for (int j = 0;j < ynum + 1;j++)for (int i = 0;i < xnum + 1;i++) {
            float *map2d = new float[size2d*size2d];
            memset(map2d, 0, size2d*size2d * sizeof(float));
            tmpdisparity.push_back(map2d);
        }
        disparity.push_back(tmpdisparity);  tmpdisparity.clear();
    }
    cout<<" Done"<<endl;
}

Disparitymap::~Disparitymap()
{
}

string Disparitymap::Validation(int nb, double thr, double m_eps, double sigma)
{
    MedianFilter(nb, thr, m_eps);
    Smoothing(sigma,nb);
    return " Done";
}

void Disparitymap::MedianFilter(int nb, double thr, double eps)
{
cout<<"DisparityMap - Median Filtering ";
#pragma region median 계산부
    int ix, iy, iz;// 참조할 위치의 인덱스
    for (int camcnt = 1; camcnt < Camnum;camcnt++) {
        for (int k = 0;k < znum;k++)for (int j = 0;j < ynum;j++)for (int i = 0;i < xnum;i++) {
        //cout<<i<<" "<<j<<" "<<k<<endl;
            vector<float> datax, datay;
            for (int kk = -nb;kk <= nb;kk++)for (int jj = -nb;jj <= nb;jj++)for (int ii = -nb;ii <= nb;ii++) {
                //cout <<"kerner "<< ii << " " << jj << " " << kk << endl;
                ix = i + ii, iy = j + jj, iz = k + kk;
                //허용되는 위치의 데이터 복사
                if (0 <= ix && ix < xnum && 0 <= iy && iy < ynum && 0 <= iz && iz < znum) {
                    if (ii != 0 && jj != 0 && kk != 0) {
                        float tmp;
                        tmp = vox[camcnt][iz][iy][ix].x, datax.push_back(tmp);
                        tmp = vox[camcnt][iz][iy][ix].y, datay.push_back(tmp);
                    }
                }
            }
            sort(datax.begin(), datax.end());
            sort(datay.begin(), datay.end());
            int Cnt = datax.size(); Point2f valmedi;
            if (Cnt % 2 == 0) {
                valmedi.x = (datax[Cnt / 2] + datax[(Cnt / 2) - 1]) / 2;
                valmedi.y = (datay[Cnt / 2] + datay[(Cnt / 2) - 1]) / 2;
            }
            else {
                valmedi.x = datax[Cnt / 2];
                valmedi.y = datay[Cnt / 2];
            }
            Point2f fluct;
            fluct.x = vox[camcnt][k][j][i].x - valmedi.x, fluct.y = vox[camcnt][k][j][i].y - valmedi.y;
            vector<float>resx, resy;
            for (int cnt = 0;cnt < Cnt;cnt++) {
                resx.push_back(datax[cnt] - valmedi.x);
                resy.push_back(datay[cnt] - valmedi.y);
            }
            sort(resx.begin(), resx.end());
            sort(resy.begin(), resy.end());
            Point2f valflucmedi;
            if (Cnt % 2 == 0) {
                valflucmedi.x = (resx[Cnt / 2] + resx[(Cnt / 2) - 1]) / 2;
                valflucmedi.y = (resy[Cnt / 2] + resy[(Cnt / 2) - 1]) / 2;
            }
            else {
                valflucmedi.x = resx[Cnt / 2];
                valflucmedi.y = resy[Cnt / 2];
            }

            float normx, normy, norm;
            normx = abs(fluct.x / (valflucmedi.x + eps));
            normy = abs(fluct.y / (valflucmedi.y + eps));
            norm = sqrt(normx*normx + normy*normy);
            if (norm > thr) {errvox[camcnt][k][j][i] = true;
            if (Cnt % 2 == 0) {
                vox[camcnt][k][j][i].x = (datax[Cnt / 2] + datax[(Cnt / 2) - 1]) / 2;
                vox[camcnt][k][j][i].y = (datay[Cnt / 2] + datay[(Cnt / 2) - 1]) / 2;
            }
            else {
                vox[camcnt][k][j][i].x = datax[Cnt / 2];
                vox[camcnt][k][j][i].y = datay[Cnt / 2];
            }
            } else {errvox[camcnt][k][j][i] = false;}


            datax.clear();  datay.clear();
            resx.clear();   resy.clear();
        }
    }
    cout << "Done" << endl;
   // Linear3DInterpolation();
}

void Disparitymap::Linear3DInterpolation()
{
    cout << "DisparityMap - Linear3DInterpolation ";
    for (int camcnt = 0;camcnt < Camnum;camcnt++) {
        for (int k = 0;k < znum;k++)for (int j = 0;j < ynum;j++)for (int i = 0;i < xnum;i++) {
            if (errvox[camcnt][k][j][i])
            {
                vector<Point2f> nonerrdata;
                if (0 <= i - 1  && !errvox[camcnt][k][j][i - 1])nonerrdata.push_back(vox[camcnt][k][j][i - 1]);
                if (i + 1 < xnum&& !errvox[camcnt][k][j][i + 1])nonerrdata.push_back(vox[camcnt][k][j][i + 1]);
                if (0 <= j - 1  && !errvox[camcnt][k][j - 1][i])nonerrdata.push_back(vox[camcnt][k][j - 1][i]);
                if (j + 1 < ynum&& !errvox[camcnt][k][j + 1][i])nonerrdata.push_back(vox[camcnt][k][j + 1][i]);
                if (0 <= k - 1  && !errvox[camcnt][k - 1][j][i])nonerrdata.push_back(vox[camcnt][k - 1][j][i]);
                if (k + 1 < xnum&& !errvox[camcnt][k + 1][j][i])nonerrdata.push_back(vox[camcnt][k + 1][j][i]);
                Point2f redata; redata.x =0, redata.y=0;
                for(int datacnt=0;datacnt<nonerrdata.size();datacnt++){
                    redata.x += nonerrdata[datacnt].x;
                    redata.y += nonerrdata[datacnt].y;
                }
                redata.x /= nonerrdata.size();                redata.y /= nonerrdata.size();
                errvox[camcnt][k][j][i]=false;
                nonerrdata.clear();
            }
        }
    }
    cout << "Done" << endl;
}

void Disparitymap::Smoothing(double sigma, int nb)
{
    cout << "DisparityMap - Smoothing ";
    float A = 1 / pow(sqrt(2 * 3.141592)*sigma, 3);
    int ix, iy, iz;// 참조할 위치의 인덱스
    for (int camcnt = 1; camcnt < Camnum;camcnt++) {
        for (int k = 0;k < znum;k++)for (int j = 0;j < ynum;j++)for (int i = 0;i < xnum;i++) {
            vector<float> kernel;
            for (int kk = -nb;kk <= nb;kk++)for (int jj = -nb;jj <= nb;jj++)for (int ii = -nb;ii <= nb;ii++) {
                ix = i + ii, iy = j + jj, iz = k + kk;
                if (0 <= ix && ix < xnum && 0 <= iy && iy < ynum && 0 <= iz && iz < znum) {
                    float tmp;
                    tmp = A * exp(-((ii)*(ii)+(jj)*(jj)+(kk)*(kk)) / (2 * sigma*sigma));
                    kernel.push_back(tmp);
                }
            }
            float sum = 0;
            for (int kercnt = 0;kercnt < kernel.size();kercnt++)sum += kernel[kercnt];
            for (int kercnt = 0;kercnt < kernel.size();kercnt++)kernel[kercnt] /= sum;

            float x = 0, y = 0; int ker_cnt = 0;
            for (int kk = -nb;kk <= nb;kk++)for (int jj = -nb;jj <= nb;jj++)for (int ii = -nb;ii <= nb;ii++) {
                ix = i + ii, iy = j + jj, iz = k + kk;
                if (0 <= ix && ix < xnum && 0 <= iy && iy < ynum && 0 <= iz && iz < znum) {
                    x += vox[camcnt][iz][iy][ix].x*kernel[ker_cnt], y += vox[camcnt][iz][iy][ix].y*kernel[ker_cnt];
                    ker_cnt++;
                }
            }
            tmpvox[k][j][i].x = x, tmpvox[k][j][i].y = y;
            kernel.clear();
        }
#pragma omp parallel for
        for (int k = 0;k < znum;k++)for (int j = 0;j < ynum;j++)for (int i = 0;i < xnum;i++)
            vox[camcnt][k][j][i] = tmpvox[k][j][i];
    }
    cout<<"Done"<<endl;
}

void Disparitymap::Write3DMap(string folderpath, int rcam, deque<int> ocam, deque<double> nodeX, deque<double> nodeY, deque<double> nodeZ)
{
    for (int cnt = 0;cnt < 3;cnt++){
        string path = folderpath + "Disparity3Dmap_" + to_string(rcam+1) + to_string(ocam[cnt]+1) +".plt";
        ofstream outfile;
        outfile.open(path, ios::trunc);
        outfile << "TITLE = \"Disparity Cam : " << to_string(rcam+1) + to_string(ocam[cnt]+1) << "\"" << endl;
        outfile << "VARIABLES = \"x\" \"y\" \"z\" \"u\" \"v\" \"w\"" << endl;
        outfile << "ZONE T= \"" << " \"" << endl;
        outfile << " I =" << xnum << ", J=" << ynum << ", K=" << znum << ", F=POINT" << endl;
        for (int k = 0;k < znum;k++) for (int j = 0;j < ynum;j++)for (int i = 0;i < xnum;i++)
            outfile << nodeX[i] << "\t" << nodeY[j] << "\t" << nodeZ[k] << "\t" <<
            vox[cnt][k][j][i].x << "\t" << vox[cnt][k][j][i].y << "\t" << 0 << endl;
        outfile.close();
    }
}