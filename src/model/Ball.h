#ifndef BALL_H
#define BALL_H

#include <glm/glm.hpp>
#include "Power.h"

class Ball
{
public:
    Ball(glm::vec2 startPos, glm::vec2 direction);
    void update(float deltaTime);
    float getRadius();
    glm::vec2 getPosition() const;
    void reflect(glm::vec2 normal, float penetration);
    void changeDirection(glm::vec2 direction);
    void deactivate();
    void activate();
    void changePosition(glm::vec2 newPosition);
    bool isActive() const;
    void setDelay(float delay) { this->delay = delay; }
    Power *getLastPower() const { return lastPower ? lastPower : nullptr; }
    void setLastPower(Power *power) { lastPower = power; }

private:
    Power *lastPower = nullptr;
    glm::vec2 position;
    glm::vec2 direction;
    float radius;
    bool active;
    float delay = 0.0f;
};

#endif
