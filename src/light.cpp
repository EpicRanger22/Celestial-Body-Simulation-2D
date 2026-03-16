#include "body.cpp"

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
        this->vX = vX;
        this->vY = vY;
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

        double accel = (3*G*mass)/(C*C*dist*dist*dist*dist*dist) * (L*L);
        
        double aX = accel * (dX/dist);
        double aY = accel * (dY/dist);

        this->vX += aX * deltaTime;
        this->vY += aY * deltaTime;

        double v = sqrt((this->vX)*(this->vX)+(this->vY)*(this->vY));
        this->vX = C*this->vX/v;
        this->vY = C*this->vY/v;
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
    double xs[NUM_LIGHT_RAYS];
    double ys[NUM_LIGHT_RAYS];

public:
    void Setup(double x, double y)
    {
        for(int i = 0; i < NUM_LIGHT_RAYS; i++)
        {
            this->xs[i] = x;
            this->ys[i] = y;
        }

        this->xs[0] = x;
        this->ys[0] = y;
    }   

    void Move(double x, double y)
    {
        for (int i = NUM_LIGHT_RAYS - 1; i > 0; i--)
        {
            this->xs[i] = this->xs[i - 1];
            this->ys[i] = this->ys[i - 1];
        }
        xs[0] = x;
        ys[0] = y;
    }

    double getX(int i) { return this->xs[i]; }
    double getY(int i) { return this->ys[i]; }
};
