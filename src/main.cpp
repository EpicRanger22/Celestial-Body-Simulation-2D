#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
//#include <list>
#include <cstdlib>
#include <cstdio>
#include <thread>
#include "../headers/constants.hpp"
#include "../headers/body.hpp"
#include "../headers/ray.hpp"
#include "../headers/light.hpp"
#include "../headers/collisions.hpp"
#include <chrono>
#include <iostream>
#include <string>

float timeScale = 1.0f;
double deltaTime = 0.0;
double activeDt = 0.0;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;

const int SCREENWIDTH = 1920;
const int SCREENHEIGHT = 1200;
const int SCREENOFFSET = 10;

bool running = true;

bool mouseDown;

bool paused = false;

double lightSpawnX = 1920/2.0;
double lightSpawnY = 1200/2.0;
double randomDirectionOffset = 3;
double spawnTimer = 0.0;
double spawnRate = 0.01;

bool lightsOut = false;

double densityRadius = 5.0;

std::vector<Light> lights;
std::vector<Ray> rays;
std::vector<Body> bodies;

double timer = 0.0;
double t = 1.0;

double planetX = 0.0;
double planetY = 0.0;

double blackHoleSchwarzschildRadius = 0.0;

std::vector<float> fpss;

int pRed;
int pGreen;
int pBlue;

double CalculateDensity(double _pX, double _pY, std::vector<Light> _lights)
{
    double pointDensity = 0.0;
    int amountInside = -1;
    for(int i = 0; i < _lights.size(); i++)
    {
        double iX = _lights[i].getX();
        double iY = _lights[i].getY();
        double dX = _pX - iX;
        double dY = _pY - iY;

        double dist = dX*dX + dY*dY;
        if (dist > densityRadius*densityRadius || dist == 0)
            continue;

        if(Collisions::insideCircle(_pX, _pY, iX, iY, densityRadius))
        {
            amountInside++;
        }
    }

    return pointDensity = amountInside;
}

void DrawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius)
{
    for (int w = -radius; w <= radius; w++)
    {
        for (int h = -radius; h <= radius; h++)
        {
            if (w*w + h*h <= radius*radius) SDL_RenderPoint(renderer, cx + w, cy + h);
        }
    }
}

void InitWindow()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Celstial Body Simulator", SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, NULL);
}

void Setup()
{
    if(bodies.size() > 0)
    {
        bodies.clear();
        lights.clear();
        rays.clear();
    }

    double gDouble = static_cast<double>(simConfig.G);
    //double cDouble = static_cast<double>(C);
    double cDouble = static_cast<double>(simConfig.C);
    //double bhMass = bodies[0].getMass();
    double bhMass = 50000000;
    //blackHoleSchwarzschildRadius = (2*gDouble*bhMass) / (cDouble * cDouble);
    blackHoleSchwarzschildRadius = 56.303770;

    if(simConfig.useSchwarzschildRadius)
    {
        blackHoleSchwarzschildRadius = (2*gDouble*simConfig.blackHoleMass) / (cDouble * cDouble);
    }
    else
    {
        blackHoleSchwarzschildRadius = simConfig.blackHoleRadius;
    }

    Body blackHole{simConfig.blackHoleX, simConfig.blackHoleY, 0, 0, blackHoleSchwarzschildRadius, simConfig.blackHoleMass, true};
    Body planet{planetX, planetY, 0, 0, simConfig.planetRadius, simConfig.planetMass, true};

    bodies.push_back(blackHole);
    bodies.push_back(planet);

    for(int i = 0; i < simConfig.numLights; i++)
    {
        Light a{0, 0, 1, 1};
        lights.push_back(a);
        Ray b{0,0};
        rays.push_back(b);
    }

    // Start lights at edges of screen
    /*for(int i = 0; i < NUMLIGHT; i++)
    {
        Light l;
        float x = 0.0;
        float y = 0.0;
        float vX = 0.0;
        float vY = 0.0;
        
        int falseI = i;
        while(falseI > 3)
        {
            falseI -= 4;
        }

        if(falseI == 0)
        {
            y = 0;
            x = i*((SCREENWIDTH + 0.0f)/NUMLIGHT);
            vY = 1;
        }
        else if(falseI == 1)
        {
            y = SCREENHEIGHT;
            x = i*((SCREENWIDTH + 0.0f)/NUMLIGHT);
            vY = -1;
        }
        else if(falseI == 2)
        {
            x = 0;
            y = i*((SCREENHEIGHT + 0.0f)/NUMLIGHT);
            vX = 1;
        }
        else
        {
            x = SCREENWIDTH;
            y = i*((SCREENHEIGHT+0.0f)/NUMLIGHT);
            vX = -1;
        }

        l.Setup(x, y, vX, vY);
        lights[i] = l;
        Ray r;
        r.Setup(-10, -10);
        rays[i] = r;
    }*/

    // Circle radius r around center going inwards/outwards
    // Starting velocity is outwards + offset
    // float r = 2*M_PI/(simConfig.numLights + 0.0f);
    // float dist = 100;
    // float offset = 0.0f;
    // double C = simConfig.C;
    // for(int i = 0; i < simConfig.numLights; i++)
    // {
    //     float x = SCREENWIDTH / 2.0;
    //     float y = SCREENHEIGHT/2.0;
    //     x += dist*cos(r*i);
    //     y += dist*sin(r*i);

    //     float vX = C*cos(r*i+offset);
    //     float vY = -C*sin(r*i+offset);
    //     Light l{x, y, -vX, -vY};
        
    //     lights.push_back(l);

    //     Ray r{-10, -10};
    //     rays.push_back(r);
    // }
}

void UpdateWindow()
{
    auto frameStart = std::chrono::high_resolution_clock::now();

    SDL_PollEvent(&event);

    if(event.type == SDL_EVENT_QUIT)
    {
        running = false;
    }
    if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        mouseDown = true;
    }
    if(event.type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        mouseDown = false;
    }
    if(event.type == SDL_EVENT_KEY_DOWN)
    {
        if(event.key.key == SDLK_COMMA) timeScale -= 0.1f;
        if(event.key.key == SDLK_PERIOD) timeScale += 0.1f;

        if(event.key.key == SDLK_Y)
        {
            // increase planetmass
            double m = bodies[1].getMass() * 2.0;
            Body b{bodies[1].getX(), bodies[1].getY(), bodies[1].getVX(), bodies[1].getVY(), bodies[1].getRadius(), m, bodies[1].isStationary()};
            bodies[1] = b;
        }
        if(event.key.key == SDLK_U)
        {
            // decrease planetmass
            double m = bodies[1].getMass() / 2.0;
            Body b{bodies[1].getX(), bodies[1].getY(), bodies[1].getVX(), bodies[1].getVY(), bodies[1].getRadius(), m, bodies[1].isStationary()};
            bodies[1] = b;
        }

        if(event.key.key == SDLK_A)
        {
            //pause
            paused = true;
        }
        if(event.key.key == SDLK_D)
        {
            //play
            paused = false;
        }
        if(event.key.key == SDLK_R)
        {
            //restart
            timer = 0;
            lightsOut = false;
            Setup();
        }
    }
    if(mouseDown)
    {
        int mouseX = event.motion.x;
        int mouseY = event.motion.y;

        if(mouseX != 0)
        {
            planetX = mouseX;
            planetY = mouseY;

            bodies[1].setPosition(planetX, planetY);
        }
    }
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    int wWidth;
    int wHeight;

    SDL_GetWindowSize(window, &wWidth, &wHeight);

    SDL_FRect windowRect = {0, 0, float(wWidth), float(wHeight)};
    SDL_RenderFillRect(renderer, &windowRect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
    for(int i = 0; i < simConfig.numBodies; i++)
    {
        for(int j = 0; j < simConfig.numBodies; j++)
        {
            if(i == j || Collisions::outOfScreen(bodies[j].getX(), bodies[j].getY())) continue;
            if(bodies[j].getX() == -10) continue;
            
            bodies[j].CalculateGravity(bodies[i].getX(), bodies[i].getY(), bodies[i].getMass(), deltaTime);
        }

        for(int j = 0; j < simConfig.numLights; j++)
        {
            if(Collisions::outOfScreen(lights[j].getX(), lights[j].getY())) continue;

            if(lights[j].getX() != -10.0)
            {
                lights[j].CalculateMotion(bodies[i].getX(), bodies[i].getY(), bodies[i].getMass(), deltaTime);
                if(i == simConfig.numBodies - 1)
                {
                    lights[j].Move(deltaTime);

                    if(!paused)
                        rays[j].Move(lights[j].getX(), lights[j].getY());
                }
            }
            else
            {
                if(i == simConfig.numBodies - 1 && !paused)
                {
                    rays[j].Move(-10,-10);
                }
            }
        }
    }

    for(int j = 0; j < simConfig.numLights; j++)
    {
        for(int k = 0; k < simConfig.numLightRays - 1; k++)
        {
            if(rays[j].getX(k) != -10 && rays[j].getX(k+1) != -10)
            {
                int4 _c = rays[j].getColor(k);
                SDL_SetRenderDrawColor(renderer, _c.r, _c.g, _c.b, _c.a);

                SDL_RenderLine(renderer, rays[j].getX(k), rays[j].getY(k), rays[j].getX(k+1), rays[j].getY(k+1));
            }
        }
    }
    
    for(int i = 0; i < simConfig.numBodies; i++)
    {
        if(Collisions::outOfScreen(bodies[i].getX(), bodies[i].getY())) continue;
        if(bodies[i].getX() == -10) continue;

        bodies[i].Move(deltaTime);

        if(i != 0)
        {
            if(Collisions::insideCircle(bodies[0].getX(), bodies[0].getY(), bodies[i].getX(), bodies[i].getY(), bodies[0].getRadius()))
            {
                //bodies.erase(i + bodies.begin());
                Body e{-10.0, -10.0, 0, 0, 0, 0, true};
                bodies[i] = e;
            }
            if(bodies[i].getMass() != 0)
            {
                SDL_SetRenderDrawColor(renderer, pRed, pBlue, pGreen, 255);
                DrawFilledCircle(renderer, bodies[i].getX(), bodies[i].getY(), bodies[i].getRadius());
            }
        }

        for(int j = 0; j < simConfig.numLights; j++)
        {
            if(lights[j].getX() == -10) continue;
            if(Collisions::outOfScreen(lights[j].getX(), lights[j].getY()))
            {
                //lights.erase(j + lights.begin());
                Light e{-10, -10, 0, 0};
                lights[j] = e;

                continue;
            }
            if(!bodies[i].isStationary()) continue; //only blackhole 'absorbs' light right now
            if(Collisions::insideCircle(bodies[i].getX(), bodies[i].getY(), lights[j].getX(), lights[j].getY(), bodies[i].getRadius()))
            {
                Light e{-10, -10, 0, 0};
                lights[j] = e;
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    DrawFilledCircle(renderer, bodies[0].getX(), bodies[0].getY(), bodies[0].getRadius());

    // SPAWN LiGHT AT "SUN"
    if(spawnTimer >= spawnRate)
    {
        spawnTimer = 0;

        double ang;

        int r = rand() % 2;
        if(r == 0)
        {
            double _rand = rand() % 1000; // 0-999
            double p = (_rand/1000)*randomDirectionOffset;
            ang = (M_PI/4) + p;
        }
        else
        {
            double _rand = rand() % 1000; // 0-999
            double p = (_rand/1000)*randomDirectionOffset;
            ang = (M_PI/4) - p;
        }

        double dirX = cos(ang);
        double dirY = sin(ang);
        Light n{lightSpawnX, lightSpawnY, dirX, dirY};
        lights.insert(lights.begin(), n);

        Ray a{-10, -10};
        rays.insert(rays.begin(), a);

        if(lights.size() > simConfig.numLights)
        {
            lights.pop_back();
            rays.pop_back();
        }
    }

    // Density and display
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // for(int w = 0; w < SCREENWIDTH; w += 5)
    // {
    //     for(int h = 0; h < SCREENHEIGHT; h += 5)
    //     {
    //         DrawFilledCircle(renderer, w, h, CalculateDensity(w, h, lights));
    //     }
    // }

    SDL_RenderPresent(renderer);

    if(!lightsOut)
    {
        int out = 0;

        for(int i = 0; i < simConfig.numLights; i++)
        {
            if(Collisions::outOfScreen(lights[i].getX(), lights[i].getY()) || lights[i].getX() == -10) out++;
        }
        if(out == simConfig.numLights) lightsOut = true;
    }

    auto frameEnd = std::chrono::high_resolution_clock::now();
    activeDt = std::chrono::duration<double>(frameEnd - frameStart).count();

    if(!paused)
    {
        deltaTime = activeDt * timeScale;
        spawnTimer += deltaTime;
    }
    else
    {
        deltaTime = 0.0;
    }

    if(!paused && lightsOut)
        timer += deltaTime;

    double fps = 1.0 / activeDt;
    fpss.push_back(fps);

    if(timer >= t)
    {
        timer = 0;
        lightsOut = false;
        Setup();
    }
}


int main(int argv, char* args[])
{
    int seed = static_cast<unsigned int>(std::time(nullptr));

    std::srand(seed);
    std::cout << "Seed: " << seed << "\n";
    
    // Set seed:
    //std::srand(1773236746);

    pRed = rand() % 256;
    pGreen = rand() % 256;
    pBlue = rand() % 256;

    Collisions::Setup(SCREENWIDTH, SCREENHEIGHT, SCREENOFFSET);

    // v = sqrt(GM((2/r)-(1/r)))

    simConfig = Configs(0);
    planetX = simConfig.planetX;
    planetY = simConfig.planetY;

    Setup();

    InitWindow();

    while(running)
    {
        UpdateWindow();
    }
    SDL_Quit();

    int totalFPS = 0;
    int numFPS = fpss.size();
    float avg = 0.0;

    auto it = fpss.begin();

    for(int i = 0; i < numFPS; i++)
    {
        totalFPS += *it;
        std::advance(it, 1);
    }

    avg = totalFPS/numFPS;

    std::cout << "\n\nAverage FPS: " << std::to_string(avg);

    return 0;
}
