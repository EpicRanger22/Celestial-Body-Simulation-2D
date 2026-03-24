#pragma once

#include <cmath>
#include <vector>
#include "../headers/configs.hpp"

typedef struct 
{
    double x, y;
    double vX, vY;
    int r, g, b;
} particle;

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
    std::vector<particle> accretionDisk;

public:
    Body(double _x, double _y, double _vX, double _vY, double _radius, double _mass, bool _stationary);

    void CalculateGravity(double _pX, double _pY, double _mass, double _dt);
    void Move(double _dt);

    void setPosition(double _x, double _y);

    void setDisk(std::vector<particle> _p);
    void setAccretionPoint(particle _p, int _i);

    particle getDisk(int _i);
    double getX();
    double getY();
    double getMass();
    double getRadius();
    double getVX();
    double getVY();
    bool isStationary();
};