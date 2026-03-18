#include "body.cpp"
#include <vector>

class Light
{
private:
    double x;
    double y;
    double startX;
    double startY;
    double vX;
    double vY;

public:
    void Setup(double x, double y, double vX, double vY)
    {
        this->x = x;
        this->y = y;
        this->startX = x;
        this->startY = y;

        float magVelocity = sqrt(vX*vX+vY*vY);
        this->vX = simConfig.C * vX/magVelocity;
        this->vY = simConfig.C * vY/magVelocity;
    }

    void CalculateMotion(double pX, double pY, double mass, double deltaTime)
    {
        if(this->x == -10) return;
        if(mass == 0) return;
        double dX = pX - this->x;
        double dY = pY - this->y;

        double dist = sqrt(dX*dX + dY*dY);
        if(dist == 0) return;

        double L = dX*this->vY - dY*this->vX;

        double accel = (3*simConfig.G*mass)/(simConfig.C*simConfig.C*dist*dist*dist*dist*dist) * (L*L);
        
        double aX = accel * (dX/dist);
        double aY = accel * (dY/dist);

        this->vX += aX * deltaTime;
        this->vY += aY * deltaTime;

        double v = sqrt((this->vX)*(this->vX)+(this->vY)*(this->vY));
        this->vX = simConfig.C*this->vX/v;
        this->vY = simConfig.C*this->vY/v;
    }

    void Move(double deltaTime)
    {
        this->x += this->vX * deltaTime;
        this->y += this->vY * deltaTime;
    }

    void setV(double vX, double vY)
    {
        this->vX = vX;
        this->vY = vY;
    }

    double getVX() { return this->vX; }
    double getVY() { return this->vY; }
    double getX() { return this->x; }
    double getY() { return this->y; }
    double getStartX() { return this->startX; }
    double getStartY() { return this->startY; }
};

class Ray
{
private:
    std::vector<double> xs;
    std::vector<double> ys;

public:
    void Setup(double x, double y)
    {
        for(int i = 0; i < simConfig.numLightRays; i++)
        {
            this->xs.push_back(x);
            this->ys.push_back(y);
        }
    }   

    void Move(double x, double y)
    {
        this->xs.insert(this->xs.begin(), x);
        if(this->xs.size() > simConfig.numLightRays) 
        {
            xs.pop_back();
        }

        this->ys.insert(this->ys.begin(), y);
        if(this->ys.size() > simConfig.numLightRays)
        {
            ys.pop_back();
        }
    }

    double getX(int i) { return this->xs[i]; }
    double getY(int i) { return this->ys[i]; }
};
