#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "light.cpp"
#include <list>
#include <cstdlib>
#include <cstdio>
#include <thread>
#include "../headers/constants.h"
#include <chrono>

float timeScale = 1.0f;
double deltaTime = 0.0;
double activeDt = 0.0;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;

const int SCREENWIDTH = 2560;
const int SCREENHEIGHT = 1600;
const int SCREENOFFSET = 10;

bool running = true;

bool mouseDown;

bool paused = false;

bool lightsOut = false;

std::vector<Light> lights;
std::vector<Ray> rays;
std::vector<Body> bodies;

double timer = 0.0;
double t = 1.0;

double planetX = 0.0;
double planetY = 0.0;

double blackHoleSchwarzschildRadius = 0.0;

std::list<float> fpss;

int pRed;
int pGreen;
int pBlue;

bool outOfScreenBody(Body body)
{
    return body.getX() > SCREENWIDTH + SCREENOFFSET || body.getX() < -SCREENOFFSET || body.getY() < -SCREENOFFSET || body.getY() > SCREENHEIGHT + SCREENOFFSET;
}
bool outOfScreenLight(Light light)
{
    return light.getX() > SCREENWIDTH + SCREENOFFSET || light.getX() < -SCREENOFFSET || light.getY() < -SCREENOFFSET || light.getY() > SCREENHEIGHT + SCREENOFFSET;
}

bool insideCircle(float cX, float cY, float x, float y, float radius)
{
    // black hole: (400, 300)
    // so at 400, 300 = (400-400)^2+(300-300)^2=0
    float circ = 0.0f;
    float rad = 0.0f;
    circ = (x-cX)*(x-cX)+(y-cY)*(y-cY);
    rad = radius * radius;

    if(circ <= rad) return true;
    return false;
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

    double scalar = 1.0f;
    
    double radiusSun = (700000000.0) * scalar;
    double mSun = (2.0*pow(10,30)) * scalar; // kg
    double blackHoleMass = 25000000000;
    double rEarthToSun = (149.6*pow(10,9)) * scalar; // million m
    double vEarth = (29784.8) * scalar; // ms^-1
    double mEarth = (5.972*pow(10, 24)) * scalar;
    double radiusEarth = (6371000.0) * scalar;

    //blackHole.Setup(400, 300, 0, 0, 20, 100000);
    Body blackHole;
    Body planet;

    double startingV = 1000;

    planet.Setup(planetX, planetY, 0, 0, simConfig.planetRadius, simConfig.planetMass, true);

    //asteroid.Setup(400, 10, 10+5, 0, 1, 1000);

    //Body asteroid;
    //asteroid.Setup(400, 200, 0, 0, 10, 200000);
    
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

    blackHole.Setup(simConfig.blackHoleX, simConfig.blackHoleY, 0, 0, blackHoleSchwarzschildRadius, simConfig.blackHoleMass, true);

    bodies.push_back(blackHole);
    bodies.push_back(planet);

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
    float r = 2*M_PI/(simConfig.numLights + 0.0f);
    float dist = 100;
    float offset = 0.0f;
    double C = simConfig.C;
    for(int i = 0; i < simConfig.numLights; i++)
    {
        Light l;
        float x = SCREENWIDTH / 2.0;
        float y = SCREENHEIGHT/2.0;
        x += dist*cos(r*i);
        y += dist*sin(r*i);

        float vX = C*cos(r*i+offset);
        float vY = -C*sin(r*i+offset);
        l.Setup(x, y, -vX, -vY);
        
        lights.push_back(l);

        Ray r;
        r.Setup(-10, -10);
        rays.push_back(r);
    }
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

        if(event.key.key == SDLK_P)
        {
            // print variables
            printf(" planetX: %f planetY: %f", planetX,planetY);
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
            if(i == j || outOfScreenBody(bodies[j])) continue;
            if(bodies[j].getX() == -10) continue;
            
            bodies[j].CalculateGravity(bodies[i].getX(), bodies[i].getY(), bodies[i].getMass(), deltaTime);
        }

        for(int j = 0; j < simConfig.numLights; j++)
        {
            if(outOfScreenLight(lights[j])) continue;

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
                // PROBLEM WITH RAY
                SDL_RenderLine(renderer, rays[j].getX(k), rays[j].getY(k), rays[j].getX(k+1), rays[j].getY(k+1));
            }
        }
    }
    
    for(int i = 0; i < simConfig.numBodies; i++)
    {
        if(outOfScreenBody(bodies[i])) continue;
        if(bodies[i].getX() == -10) continue;

        bodies[i].Move(deltaTime);

        if(i != 0)
        {
            if(insideCircle(bodies[0].getX(), bodies[0].getY(), bodies[i].getX(), bodies[i].getY(), bodies[0].getRadius()))
            {
                Body e;
                e.Setup(-10.0, -10.0, 0, 0, 0, 0, true);
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
            if(outOfScreenLight(lights[j]))
            {
                Light e;
                e.Setup(-10, -10, 0, 0);
                lights[j] = e;

                continue;
            }
            if(!bodies[i].isStationary()) continue; //only blackhole 'absorbs' light right now
            if(insideCircle(bodies[i].getX(), bodies[i].getY(), lights[j].getX(), lights[j].getY(), bodies[i].getRadius()))
            {
                Light e;
                e.Setup(-10, -10, 0, 0);
                lights[j] = e;
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    DrawFilledCircle(renderer, bodies[0].getX(), bodies[0].getY(), bodies[0].getRadius());

    SDL_RenderPresent(renderer);

    if(!lightsOut)
    {
        int out = 0;

        for(int i = 0; i < simConfig.numLights; i++)
        {
            if(outOfScreenLight(lights[i]) || lights[i].getX() == -10) out++;
        }
        if(out == simConfig.numLights) lightsOut = true;
    }

    auto frameEnd = std::chrono::high_resolution_clock::now();
    activeDt = std::chrono::duration<double>(frameEnd - frameStart).count();

    if(!paused)
        deltaTime = activeDt * timeScale;
    else
        deltaTime = 0.0;

    if(!paused && lightsOut)
        timer += activeDt;

    double fps = 1.0 / activeDt;
    fpss.push_front(fps);

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
