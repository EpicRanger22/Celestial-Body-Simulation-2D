#include "../headers/ray.hpp"

Ray::Ray(double _x, double _y)
{
    for(int i = 0; i < simConfig.numLightRays; i++)
    {
        xs.push_back(_x);
        ys.push_back(_y);
    }
}   

void Ray::Move(double _x, double _y)
{
    xs.insert(xs.begin(), _x);
    if(xs.size() > simConfig.numLightRays) 
    {
        xs.pop_back();
    }

    ys.insert(ys.begin(), _y);
    if(ys.size() > simConfig.numLightRays)
    {
        ys.pop_back();
    }
}

double Ray::getX(int _i) { return xs[_i]; }
double Ray::getY(int _i) { return ys[_i]; }