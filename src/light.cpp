#include "../headers/light.hpp"

Light::Light(double _x, double _y, double _vX, double _vY)
{
    x = _x;
    y = _y;
    startX = _x;
    startY = _y;

    float magVelocity = sqrt(_vX*_vX+_vY*_vY);
    vX = simConfig.C * _vX/magVelocity;
    vY = simConfig.C * _vY/magVelocity;
}

void Light::CalculateMotion(double _pX, double _pY, double _mass, double _dt)
{
    if(x == -10) return;
    if(_mass == 0) return;
    double dX = _pX - x;
    double dY = _pY - y;

    double dist = sqrt(dX*dX + dY*dY);
    if(dist == 0) return;

    double L = dX*vY - dY*vX;

    double accel = (3*simConfig.G*_mass)/(simConfig.C*simConfig.C*dist*dist*dist*dist*dist) * (L*L);
    
    double aX = accel * (dX/dist);
    double aY = accel * (dY/dist);

    vX += aX * _dt;
    vY += aY * _dt;

    double v = sqrt((vX)*(vX)+(vY)*(vY));
    vX = simConfig.C*vX/v;
    vY = simConfig.C*vY/v;
}

void Light::Move(double _dt)
{
    x += vX * _dt;
    y += vY * _dt;
}

void Light::setV(double _vX, double _vY)
{
    vX = _vX;
    vY = _vY;
}

double Light::getVX() { return vX; }
double Light::getVY() { return vY; }
double Light::getX() { return x; }
double Light::getY() { return y; }
double Light::getStartX() { return startX; }
double Light::getStartY() { return startY; }