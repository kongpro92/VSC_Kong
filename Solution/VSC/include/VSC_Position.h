#ifndef VSC_POSITION_H
#define VSC_POSITION_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
class Position {
public:
  Position() {};
  Position(double newX,double newY, double newZ);
  ~Position() {};
  
	
public:
  double x;
  double y;
  double z;
  double xref;
  double yref;
  double x1;
  double y1;
  double x2;
  double y2;
  double x3;
  double y3;
  double inten;
  double Xr1, Yr1, Zr1;
  double Xr2, Yr2, Zr2;
  double Xr3, Yr3, Zr3;

  double X() const;     // get x component
  double Y() const;     // get y component
  double Z() const;     // get z component

  friend Position Multiply(Position& left, Position& right);
  friend Position Devide(Position& left, Position& right);
  friend Position Inverse(Position& p);
  friend Position operator+(Position& left, Position& right);
  friend Position operator-(Position& left, Position& right);
  friend Position operator/(Position& left, double right);
  friend Position operator*(Position& left, double right);
  friend Position operator*(double left, Position& right);
};


#endif // POSITION_H
