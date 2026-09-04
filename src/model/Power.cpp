#include "Power.h"
#include <random>

Power::Power(GridObjectType type, glm::vec2 position)
    : GridObject(type, position) // Valore di default
{
}

PowerType Power::getPowerType() const
{
    return powerType;
}

void Power::deactivate()
{
    active = false;
}

bool Power::isActive() const
{
    return active;
}

void Power::initPower(PowerType type)
{
    this->powerType = type;
    this->active = true;
}

std::chrono::steady_clock::time_point Power::getLaserTime() const
{
    return laserTime;
}

void Power::setLaserTime()
{
    laserTime = std::chrono::steady_clock::now();
}