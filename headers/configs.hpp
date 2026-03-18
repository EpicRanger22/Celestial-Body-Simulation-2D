#pragma once
#ifndef CONFIGS_H
#define CONFIGS_H

struct SimulationConfig
{
    double G;
    double C;
    int numBodies;
    int numLights;
    int numLightRays;

    bool useSchwarzschildRadius;

    double blackHoleX;
    double blackHoleY;
    double blackHoleMass;
    double blackHoleRadius;

    double planetX;
    double planetY;
    double planetMass;
    double planetRadius;
};

SimulationConfig Configs(int cfgIndex);

extern SimulationConfig simConfig;

#endif