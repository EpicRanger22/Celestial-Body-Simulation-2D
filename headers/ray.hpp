#pragma once

#include <vector>
#include "configs.hpp"

class Ray
{
private:
    std::vector<double> xs;
    std::vector<double> ys;

public:
    Ray(double _x, double _y);
    void Move(double _x, double _y);

    double getX(int _i);
    double getY(int _i);
};