#include "../headers/configs.hpp"
#include <iostream>
#include <fstream>
#include <string>

SimulationConfig simConfig;

SimulationConfig Configs(int cfgIndex)
{
    SimulationConfig cfg;

    std::ifstream file("configs/cfg.txt");
    if(!file.is_open())
    {
        std::cerr << "Failed to open config file\n";
    }

    std::string line;
    int l = 0;
    bool startCode = true;
    while(std::getline(file, line))
    {
        if(line[0] == '#' && line[1] == cfgIndex)
        {
            startCode = true;
        }

        if(startCode)
        {
            if(l == 1) cfg.G = std::stod(line);
            if(l == 2) cfg.C = std::stod(line);
            if(l == 3) cfg.numBodies = std::stoi(line);
            if(l == 4) cfg.numLights = std::stoi(line);
            if(l == 5) cfg.numLightRays = std::stoi(line);
            if(l == 6) cfg.useSchwarzschildRadius = std::stoi(line);
            if(l == 7) cfg.blackHoleX = std::stod(line);
            if(l == 8) cfg.blackHoleY = std::stod(line);
            if(l == 9) cfg.blackHoleMass = std::stod(line);
            if(l == 10) cfg.blackHoleRadius = std::stod(line);
            if(l == 11) cfg.planetX = std::stod(line);
            if(l == 12) cfg.planetY = std::stod(line);
            if(l == 13) cfg.planetMass = std::stod(line);
            if(l == 14) cfg.planetRadius = std::stod(line);

            l++;
        }

        if(line == "")
        {
            startCode = false;
        }
    }

    //cfg.G = std::stod()

    file.close();

    return cfg;
}