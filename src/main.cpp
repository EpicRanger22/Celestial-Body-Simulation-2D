#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "light.cpp"
#include <list>
#include <cstdlib>
#include <cstdio>
#include <thread>
#include <chrono>

const int TARGET_FPS = 60;
const Uint64 FRAME_DELAY = 1000 / TARGET_FPS;
float timeScale = 1.0f;
double deltaTime = 0.0f;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;

bool running = true;

const int NUMBODIES = 2;
const int NUMLIGHT = 300;
Light lights[NUMLIGHT];
Ray rays[NUMLIGHT];
Body bodies[NUMBODIES];

double blackHoleSchwarzschildRadius = 0.0;

std::list<float> fpss;

int pRed;
int pGreen;
int pBlue;

// const int SIZE = 200;
// int xs[SIZE];
// int ys[SIZE];

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

    window = SDL_CreateWindow("Celstial Body Simulator", 800, 600, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, NULL);
}

void UpdateWindow()
{
    Uint64 frameStart = SDL_GetTicks();
    auto frameStartA = std::chrono::high_resolution_clock::now();

    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    int wWidth;
    int wHeight;

    SDL_GetWindowSize(window, &wWidth, &wHeight);

    SDL_FRect windowRect = {0, 0, float(wWidth), float(wHeight)};
    SDL_RenderFillRect(renderer, &windowRect);

    // for (int i = SIZE - 1; i > 0; i--)
    // {
    //     xs[i] = xs[i - 1];
    //     ys[i] = ys[i - 1];
    // }

    // xs[0] = bodies[1].getX();
    // ys[0] = bodies[1].getY();

    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // for(int i = 0; i < SIZE - 1; i++)
    // {
    //     if(xs[i] != 0 && xs[i+1] != 0)
    //         SDL_RenderLine(renderer, xs[i], ys[i], xs[i+1],ys[i+1]);
    // }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
    for(int i = 0; i < NUMBODIES; i++)
    {
        for(int j = 0; j < NUMBODIES; j++)
        {
            if(i == j || bodies[j].getX() > 1000 || bodies[j].getX() < -200 || bodies[j].getY() < - 200 || bodies[j].getY() > 800) continue;
            if(bodies[j].getX() == -10) continue;
            
            bodies[j].CalculateGravity(bodies[i].getX(), bodies[i].getY(), bodies[i].getMass(), deltaTime);
        }

        for(int j = 0; j < NUMLIGHT; j++)
        {
            if(lights[j].getX() > 1000 || lights[j].getX() < -200 || lights[j].getY() < - 200 || lights[j].getY() > 800) continue;

            if(lights[j].getX() != -10.0)
            {
                lights[j].CalculateMotion(bodies[i].getX(), bodies[i].getY(), bodies[i].getMass(), deltaTime);
                if(i == NUMBODIES - 1)
                {
                    lights[j].Move(deltaTime);

                    rays[j].Move(lights[j].getX(), lights[j].getY());
                }
            }
            else
            {
                if(i == NUMBODIES - 1)
                {
                    rays[j].Move(-10,-10);
                }
            }

            for(int k = 0; k < NUM_LIGHT_RAYS - 1; k++)
            {
                if(rays[j].getX(k) != -10 && rays[j].getX(k+1) != -10)
                {
                    SDL_RenderLine(renderer, rays[j].getX(k), rays[j].getY(k), rays[j].getX(k+1), rays[j].getY(k+1));
                }
            }
        }
    }
    
    for(int i = 0; i < NUMBODIES; i++)
    {
        if(bodies[i].getX() > 1000.0 || bodies[i].getX() < -200.0 || bodies[i].getY() < - 200.0 || bodies[i].getY() > 800.0) continue;
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

        for(int j = 0; j < NUMLIGHT; j++)
        {
            if(lights[j].getX() == -10) continue;
            if(lights[j].getX() > 1000.0 || lights[j].getX() < -200.0 || lights[j].getY() < - 200.0 || lights[j].getY() > 800.0)
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

    SDL_PollEvent(&event);

    if(event.type == SDL_EVENT_QUIT)
    {
        running = false;
    }
    if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        int mouseX = event.motion.x;
        int mouseY = event.motion.y;

        Body n;
        n.Setup(mouseX, mouseY, 0, 0, 10, 10000, true);
        int i = rand()%NUMBODIES;
        if(i == 0) i = 1;
        bodies[i] = n;
    }
    if(event.type == SDL_EVENT_KEY_DOWN)
    {
        if(event.key.key == SDLK_COMMA) timeScale -= 0.5f;
        if(event.key.key == SDLK_PERIOD) timeScale += 0.5f;

        if(event.key.key == SDLK_P)
        {
            // print variables
            for(int i = 0; i < NUMLIGHT; i++)
            {
                if(lights[i].getX() != -10.0)
                    std::cout << " > " << std::to_string(lights[i].getStartX()) << " , " << std::to_string(lights[i].getStartY());
            }
        }
        if(event.key.key == SDLK_A)
        {
            //pause
            deltaTime = 0.0;
        }
        if(event.key.key == SDLK_D)
        {
            //play
            deltaTime = 1/60.0;
        }
    }

    Uint64 frameTime = SDL_GetTicks() - frameStart;
    auto frameEnd = std::chrono::high_resolution_clock::now();
    double dt = std::chrono::duration<double>(frameEnd - frameStartA).count();
    double fps = 1.0 / dt;
    fpss.push_front(fps);

    if (frameTime < FRAME_DELAY) {
        SDL_Delay(FRAME_DELAY - frameTime);
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

    planet.Setup(400, 250, 0, 0, 10, blackHoleMass, true);

    //asteroid.Setup(400, 10, 10+5, 0, 1, 1000);

    //Body asteroid;
    //asteroid.Setup(400, 200, 0, 0, 10, 200000);
    
    double gDouble = static_cast<double>(G);
    //double cDouble = static_cast<double>(C);
    double cDouble = 298;
    //double bhMass = bodies[0].getMass();
    double bhMass = 50000000;
    //blackHoleSchwarzschildRadius = (2*gDouble*bhMass) / (cDouble * cDouble);
    blackHoleSchwarzschildRadius = 56.303770;
    //std::cout << std::to_string(blackHoleSchwarzschildRadius);

    blackHole.Setup(400, 350, 0, 0, blackHoleSchwarzschildRadius, blackHoleMass, true);

    bodies[0] = blackHole;
    bodies[1] = planet;

    // for(int i = 1; i < 300; i++)
    // {
    //     Body l;
    //     float vX = 200;
    //     l.Setup(0, i*2, vX, 0, 1, 0.001f);
    //     bodies[i] = l;
    // }

    for(int i = 0; i < NUMLIGHT; i++)
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
            x = i*(800.0f/NUMLIGHT);
            vY = 1;
        }
        else if(falseI == 1)
        {
            y = 600;
            x = i*(800.0f/NUMLIGHT);
            vY = -1;
        }
        else if(falseI == 2)
        {
            x = 0;
            y = i*(600.0f/NUMLIGHT);
            vX = 1;
        }
        else
        {
            x = 800;
            y = i*(600.0f/NUMLIGHT);
            vX = -1;
        }

        l.Setup(x, y, vX, vY);
        lights[i] = l;
        Ray r;
        r.Setup(-10, -10);
        rays[i] = r;
    }

    // Light l;
    // float vX = 1;
    // l.Setup(1, 10, vX, 0);
    // lights[0] = l;

    // Ray r;
    // r.Setup(1, 10);
    // rays[0] = r;

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
