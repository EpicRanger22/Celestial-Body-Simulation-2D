#include "../headers/collisions.hpp"

namespace Collisions
{
    int screenWidth;
    int screenHeight;
    int screenOffset;

    void Setup(int _screenWidth, int _screenHeight, int _offset)
    {
        screenWidth = _screenWidth;
        screenHeight = _screenHeight;
        screenOffset = _offset;
    }

    bool outOfScreen(double _x, double _y)
    {
        return _x > screenWidth + screenOffset || _x < -screenOffset || _y > screenHeight + screenOffset || _y < -screenOffset;
    }
    bool insideCircle(double _cX, double _cY, double _x, double _y, double _radius)
    {
        float circ = 0.0f;
        float rad = 0.0f;
        circ = (_x-_cX)*(_x-_cX)+(_y-_cY)*(_y-_cY);
        rad = _radius * _radius;

        if(circ <= rad) return true;
        return false;
    }
}