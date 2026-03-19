#pragma once

#include <cmath>
#include <vector>
#include "configs.hpp"

class Light
{
private:
    double x;
    double y;
    double startX;
    double startY;
    double vX;
    double vY;

public:
    Light(double _x, double _y, double _vX, double _vY);
    void CalculateMotion(double _pX, double _pY, double _mass, double _dt);
    void Move(double _dt);

    void setV(double _vX, double _vY);

    double getVX();
    double getVY();
    double getX();
    double getY();
    double getStartX();
    double getStartY();
};