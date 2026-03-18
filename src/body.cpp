#include <cmath>
#include <iostream>
#include "SDL3/SDL.h"
#include <sstream>
#include "../headers/configs.h"

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
    void Setup(double x, double y, double velX, double velY, double radius, double mass, bool stationary)
    {
        this->x = x;
        this->y = y;
        this->vX = velX;
        this->vY = velY;
        this->radius = radius;
        this->mass = mass;
        this->stationary = stationary;
    }

    // SDL_FRect Render()
    // {
    //     SDL_FRect rect = {this->x-this->radius/2, this->y-this->radius/2, this->radius, this->radius};
    //     return rect;
    // }

    void CalculateGravity(double pX, double pY, double mass, double deltaTime)
    {
        if(this->stationary) return;
        if(mass == 0) return;
        double dX = pX - this->x;
        double dY = pY - this->y;

        double dist = sqrt(dX*dX + dY*dY);
        if(dist == 0) return;

        double nX = dX / dist;
        double nY = dY / dist;

        double accel = simConfig.G * mass / (dist*dist);

        double aX = accel * nX;
        double aY = accel * nY;

        this->vX += aX * deltaTime;
        this->vY += aY * deltaTime;
    }

    void Move(double deltaTime)
    {
        if(this->stationary) return;
        this->x += this->vX * deltaTime;
        this->y += this->vY * deltaTime;        
    }

    void setPosition(double x, double y)
    {
        this->x = x;
        this->y = y;
    }

    double getX() { return this->x; }
    double getY() { return this->y; }
    double getMass() { return this->mass; }
    double getRadius() { return this->radius; }
    double getVX() { return this->vX; }
    double getVY() { return this->vY; }

    bool isStationary() { return this->stationary; }
};
