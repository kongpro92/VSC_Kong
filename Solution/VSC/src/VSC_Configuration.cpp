#include <VSC_Configuration.h>

#define GETLINE(l) getline(in, ##l), ##l.erase(##l.find_first_of(' '));

Configuration::Configuration()
{
    ProjFolder = "";  
    Camcnt = 0;
    S_FrameNum = 0, E_FrameNum = 0, Framecnt =0;
    Xmin = 0, Xmax = 0, Ymin = 0, Ymax = 0, Zmin = 0, Zmax = 0;
    Xgap = 0, Ygap = 0, Zgap = 0;
    Xnum = 0, Ynum = 0, Znum = 0;

}

Configuration::Configuration(string path, int camcnt, int sfn, int efn, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
	ProjFolder = path;
	Camcnt = camcnt;
	S_FrameNum = sfn, E_FrameNum = efn, Framecnt = 0;
	Xmin = xmin, Xmax = xmax, Ymin = ymin, Ymax = ymax, Zmin = zmin, Zmax = zmax;
	Xgap = 0, Ygap = 0, Zgap = 0;
	Xnum = 0, Ynum = 0, Znum = 0;
}
void Configuration::MakeConfigurationFile()
{
    bool iter = true;
    while (iter) {
        cout << "Plaese Set your Configuration Property" << endl;
        cout << "1. Project Folder : " << ProjFolder << endl;
        cout << "2. Camera Number : " << Camcnt << endl;
        cout << "3. Start Frame Number : " << S_FrameNum << endl;
        cout << "4. End frame Numbrer : " << E_FrameNum << endl;
        cout << "5. Range of Volume" << endl;
        cout << Xmin << " ~ " << Xmax << endl;
        cout << Ymin << " ~ " << Ymax << endl;
        cout << Zmin << " ~ " << Zmax << endl;
        int mode = 0;
        cout << "Please Modify Property(if it is already done, please insert 0) : ";
        cin >> mode; cin.clear();

        bool b = true;
        switch (mode) {
        case 0: iter = false; break;
        case 1:
            while (b) {
                cin >> ProjFolder;cin.clear();
                if (_access(ProjFolder.c_str(), 0)) { cout << "There is no directory" << endl; }
                else b = false;
            }break;
        case 2:cin >> Camcnt;cin.clear(); break;
        case 3:cin >> S_FrameNum; cin.clear(); break;
        case 4:cin >> E_FrameNum; cin.clear();  break;
        case 5:
            while (b) {
                cout << "Select Range of Volume(0. end, 1. Xmin, 2. Xmax, 3.Ymin, 4.Ymax, 5.Zmin, 6.Zmax) : ";
                cin >> mode;
                switch (mode)
                {
                case 0: b = false;break;
                case 1:cout << "Xmin : "; cin >> Xmin;cin.clear(); break;
                case 2:cout << "Xmax : "; cin >> Xmax;cin.clear(); break;
                case 3:cout << "Ymin : "; cin >> Ymin;cin.clear(); break;
                case 4:cout << "Ymax : "; cin >> Ymax;cin.clear(); break;
                case 5:cout << "Zmin : "; cin >> Zmin;cin.clear();break;
                case 6:cout << "Zmax : "; cin >> Zmax;cin.clear(); break;

                }
            }b = true;break;
        }
        system("cls");
    }

    string conffilename = ProjFolder + "\\Configuration.txt";

    string out_line;
    ofstream outfile;
    outfile.open(conffilename, ios::trunc);
    outfile << ProjFolder << "\\ //Project Folder" << endl;
    outfile << Camcnt << " //Number of Camera" << endl;
    outfile << S_FrameNum << " //Start Frame Number" << endl;
    outfile << E_FrameNum << " //End Frame Number" << endl;
    for (int cnt = 0;cnt < Camcnt;cnt++)
        outfile << ProjFolder << "\\cam" << to_string(cnt) << "ImageNames.txt" << " //Image List File Path" << endl;
    outfile << Xmin << " //Xmin" << endl;
    outfile << Xmax << " //Xmax" << endl;
    outfile << Ymin << " //Ymin" << endl;
    outfile << Ymax << " //Ymax" << endl;
    outfile << Zmin << " //Zmin" << endl;
    outfile << Zmax << " //Zmax" << endl;
    for (int cnt = 0;cnt < Camcnt;cnt++)
        outfile << ProjFolder << "\\Caldata_" << to_string(cnt) << ".dat" << " //Image List File Path" << endl;
    outfile.close();
}
void Configuration::ReadConfigurationFile(string path)
{
    ifstream in(path, ios::in);
    string line;
    if (in.is_open()) {
		GETLINE(line)      ProjFolder = (line.c_str());
		GETLINE(line)      Camcnt = atoi(line.c_str());
		GETLINE(line)      S_FrameNum = atoi(line.c_str());
		GETLINE(line)      E_FrameNum = atoi(line.c_str());

        Framecnt = E_FrameNum - S_FrameNum + 1;

        for (int cnt = 0;cnt < Camcnt;cnt++) {
            getline(in, line);
            line.erase(line.find_first_of(' '));
            ImageNameListFile.push_back(line);

            //이미지 파일 경로 세팅
            deque<string> tmppath; int imgcnt = 0;
            ifstream in_(line, ios::in);
            while (!in_.eof()) {
                imgcnt++; getline(in_, line);
                if (S_FrameNum <= imgcnt && imgcnt <= E_FrameNum)
                    tmppath.push_back(ProjFolder + line);
            }
            ImagePath.push_back(tmppath);

        }

		GETLINE(line)       Xmin = atoi(line.c_str());
		GETLINE(line)       Xmax = atoi(line.c_str());
		GETLINE(line)       Ymin = atoi(line.c_str());
		GETLINE(line)       Ymax = atoi(line.c_str());
		GETLINE(line)       Zmin = atoi(line.c_str());
		GETLINE(line)       Zmax = atoi(line.c_str());


        for (int cnt = 0;cnt < Camcnt;cnt++) {
            getline(in, line);
            line.erase(line.find_first_of(' '));
            CalibrationFilePath.push_back(line);
        }

        //cout << "\b\b\b - Done" << endl;
        ShowConfiguration();
        in.close();
    }
    else {
        cout << endl << "There is no Configuration File, Plaese retry." << endl;
        return;
    }
}
void Configuration::ReadVSCConditionFile()
{
    string path = ProjFolder + "VSC_Property.txt";
    ifstream in(path, ios::in);
    string line;
    if (in.is_open()) {
        getline(in, line); line.erase(line.find_first_of(' '));
        Xnum = atoi(line.c_str());

        getline(in, line); line.erase(line.find_first_of(' '));
        Ynum = atoi(line.c_str());

        getline(in, line); line.erase(line.find_first_of(' '));
        Znum = atoi(line.c_str());

        getline(in, line);
        line.erase(line.find_first_of(' '));
        eps = atof(line.c_str());

        Xgap = (Xmax - Xmin) / (Xnum - 1); Ygap = (Ymax - Ymin) / (Ynum - 1); Zgap = (Zmax - Zmin) / (Znum - 1);

        ShowVSCCondition();
    }
    else{
        cout << endl << "There is no VSC Condition File, Plaese retry." << endl;
        return;
    }
}
void Configuration::ShowConfiguration()
{
    cout << "\tConfiguration Property" << endl;
    cout << "\t1. Project Folder     : " << ProjFolder << endl;
    cout << "\t2. Camera Number      : " << Camcnt << endl;
    cout << "\t3. Start Frame Number : " << S_FrameNum << endl;
    cout << "\t4. End frame Numbrer  : " << E_FrameNum << endl<<endl;
    
    cout << "\t\t ___________________________________________________________ " << endl;
    cout << "\t\t|                     Mesurement Volume                     |" << endl;
    cout << "\t\t|___________________________________________________________|" << endl;
    cout << "\t\t|         X         |         Y         |         Z         |" << endl;
    cout << "\t\t|-------------------|-------------------|-------------------|" << endl;
    cout << "\t\t|";
    cout.fill(' '), cout << fixed, cout.width(6), cout.precision(1), cout << Xmin << "mm ~ ";
    cout.fill(' '), cout << fixed, cout.width(6), cout.precision(1), cout << Xmax << "mm|";
    cout.fill(' '), cout << fixed, cout.width(6), cout.precision(1), cout << Ymin << "mm ~ ";
    cout.fill(' '), cout << fixed, cout.width(6), cout.precision(1), cout << Ymax << "mm|";
    cout.fill(' '), cout << fixed, cout.width(6), cout.precision(1), cout << Zmin << "mm ~ ";
    cout.fill(' '), cout << fixed, cout.width(6), cout.precision(1), cout << Zmax << "mm|" << endl;
    cout << "\t\t|___________________|___________________|___________________|" << endl << endl;
}
void Configuration::ShowVSCCondition()
{
    cout << "\tVSC Condition Property" << endl << endl;
    cout << "\t1. Epsilon     : ";
    cout.fill(' '), cout << fixed, cout.width(2), cout.precision(1), cout << eps << "(pixel)" << endl;
    cout << "\t2. Sub-Volume" << endl<<endl;
    cout << "\t\t ___________________________________________________________" << endl;
    cout << "\t\t|         X         |         Y         |         Z         |" << endl;
    cout << "\t--------|-------------------|-------------------|-------------------|" << endl;
    cout << "\t| Count |        ";
                        cout.fill(' '), cout << fixed, cout.width(3), cout << Xnum << "        |";
    cout << "        "; cout.fill(' '), cout << fixed, cout.width(3), cout << Ynum << "        |";
    cout << "        "; cout.fill(' '), cout << fixed, cout.width(3), cout << Znum << "        |"<<endl;

    cout << "\t--------|-------------------|-------------------|-------------------|" << endl;
    cout << "\t|  Gap  |       ";
    cout.fill(' '),   cout << fixed, cout.width(3), cout.precision(1), cout << Xgap << "mm       |";
    cout << "       ";cout << fixed, cout.width(3), cout.precision(1), cout << Ygap << "mm       |";
    cout << "       ";cout << fixed, cout.width(3), cout.precision(1), cout << Zgap << "mm       |"<<endl;
    cout << "\t________|___________________|___________________|___________________|" << endl << endl;
}
