#include "../headers/body.hpp"

Body::Body(double _x, double _y, double _vX, double _vY, double _radius, double _mass, bool _stationary)
{
    x = _x;
    y = _y;
    vX = _vX;
    vY = _vY;
    radius = _radius;
    mass = _mass;
    stationary = _stationary;
}

void Body::CalculateGravity(double _pX, double _pY, double _mass, double _dt)
{
    if(stationary) return;
    if(mass == 0) return;
    double dX = _pX - x;
    double dY = _pY - y;

    double dist = sqrt(dX*dX + dY*dY);
    if(dist == 0) return;

    double nX = dX / dist;
    double nY = dY / dist;

    double accel = simConfig.G * _mass / (dist*dist);

    double aX = accel * nX;
    double aY = accel * nY;

    vX += aX * _dt;
    vY += aY * _dt;
}

void Body::Move(double _dt)
{
    if(stationary) return;
    x += vX * _dt;
    y += vY * _dt;        
}

void Body::setPosition(double _x, double _y)
{
    x = _x;
    y = _y;
}

double Body::getX() { return x; }
double Body::getY() { return y; }
double Body::getMass() { return mass; }
double Body::getRadius() { return radius; }
double Body::getVX() { return vX; }
double Body::getVY() { return vY; }

bool Body::isStationary() { return stationary; }