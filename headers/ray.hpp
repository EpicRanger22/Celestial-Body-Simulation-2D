#pragma once

#include <vector>
#include "configs.hpp"

typedef struct {int r, g, b, a;} int4;

class Ray
{
private:
    std::vector<int4> color;
    std::vector<double> xs;
    std::vector<double> ys;

public:
    Ray(double _x, double _y);
    void Move(double _x, double _y);

    double getX(int _i);
    double getY(int _i);
    int4 getColor(int _i);
};