#ifndef POWER_H
#define POWER_H
#include <glm/glm.hpp>
#include "GridObject.h"
#include <chrono>

enum class PowerType
{
    LaserHorizontal,
    LaserVertical,
    AddBall
};

class Power : public GridObject
{
public:
    Power(GridObjectType type, glm::vec2 position);
    void initPower(PowerType type);
    PowerType getPowerType() const;
    void deactivate();
    bool isActive() const override;
    std::chrono::steady_clock::time_point getLaserTime() const;
    void setLaserTime();

private:
    PowerType powerType;
    bool active = false;
    std::chrono::steady_clock::time_point laserTime;
};
#endif