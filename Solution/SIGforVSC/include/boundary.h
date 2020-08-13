#ifndef BOUNDARY_H
#define BOUNDARY_H


class Boundary
{
public:
    Boundary();
    Boundary(double min_x, double min_y, double min_z, double max_x, double max_y, double max_z);


    double minx=0;
    double miny=0;
    double minz=0;
    double maxx=0;
    double maxy=0;
    double maxz=0;

};

#endif // BOUNDARY_H
