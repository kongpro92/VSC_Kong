#include <VSC_Position.h>

Position::Position(double newX, double newY, double newZ)
    : x(newX), y(newY), z(newZ)
{}

double Position::X() const
{
    return x;
}

double Position::Y() const
{
    return y;
}

double Position::Z() const
{
    return z;
}

 Position Multiply( Position& left,  Position& right)
{
    return Position(left.x * right.x, left.y * right.y, left.z * right.z);
}


 Position Inverse( Position& p)
{
    return Position(1/p.X(),1/p.Y(),1/p.Z());
}

 Position operator+( Position& left,  Position& right)
{
    return Position(left.x + right.x, left.y + right.y, left.z + right.z);
}

 Position operator-( Position& left,  Position& right)
{
    return Position(left.x - right.x, left.y - right.y, left.z - right.z);
}

 Position operator/( Position& left, double right)
{
    return Position(left.x / right, left.y / right, left.z / right);
}

Position operator*(Position& left, double right)
{
    return Position(left.x * right, left.y * right, left.z * right);
}

Position operator*(double left, Position& right)
{
    return Position(left * right.x, left * right.y, left * right.z);
}

 Position Devide( Position& left,  Position& right) 
{
    return Position(left.x / right.x, left.y / right.y, left.z / right.z);
}