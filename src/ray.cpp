#include "../headers/ray.hpp"

Ray::Ray(double _x, double _y)
{
    gone = false;

    for(int i = 0; i < simConfig.numLightRays; i++)
    {
        xs.push_back(_x);
        ys.push_back(_y);
        int4 _c;
        if(simConfig.numLightRays > 255)
        {

        }
        else
        {
            // Fade into background not black
            if(i <= 20)
            {
                _c = {20, 20, 20, 20};
            }
            else
            {
                _c = {i, i, i, i};
            }
            color.insert(color.begin(), _c);
        }
    }
}   

void Ray::Move(double _x, double _y)
{
    if(gone) return;
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

void Ray::setGone()
{
    gone = true;

    xs.clear();
    ys.clear();
    for(int i = 0; i < simConfig.numLightRays; i++)
    {
        xs.push_back(-10);
        ys.push_back(-10);
    }
}

double Ray::getX(int _i) { return xs[_i]; }
double Ray::getY(int _i) { return ys[_i]; }
int4 Ray::getColor(int _i) { return color[_i]; }