#pragma once

#include <cmath>
#include "../headers/configs.hpp"

class Body
{
private:
    double x;
    double y;
    double vX;
    double vY;
    double radius;
    double mass;
    bool stationary;

public:
    Body(double _x, double _y, double _vX, double _vY, double _radius, double _mass, bool _stationary);

    void CalculateGravity(double _pX, double _pY, double _mass, double _dt);
    void Move(double _dt);

    void setPosition(double _x, double _y);

    double getX();
    double getY();
    double getMass();
    double getRadius();
    double getVX();
    double getVY();
    bool isStationary();
};