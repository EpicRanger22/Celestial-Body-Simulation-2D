#pragma once

namespace Collisions
{
    extern int screenWidth;
    extern int screenHeight;
    extern int screenOffset;

    void Setup(int _screenWidth, int _screenHeight, int _offset);
    bool outOfScreen(double _x, double _y);
    bool insideCircle(double _cX, double _cY, double _x, double _y, double _radius);
}