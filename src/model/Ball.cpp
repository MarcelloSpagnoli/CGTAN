#include "Ball.h"

#include <glm/geometric.hpp>
#include <iostream>
#include "../configs.h"

Ball::Ball(glm::vec2 startPos, glm::vec2 direction)
    : position(startPos), direction(direction), radius(BALL_RADIUS), active(false) {}

void Ball::update(float deltaTime)
{
    if (!active)
        return; // Evita calcoli inutili se è già spenta

    if (delay > 0.0f)
    {
        delay -= deltaTime;
        return;
    }

    this->position += this->direction * deltaTime * BALL_SPEED;

    // Se il bordo inferiore della palla scende sotto lo zero (o una soglia limite)
    if (this->position.y - this->radius < 0.0f)
    {
        this->deactivate();
    }
}

void Ball::reflect(glm::vec2 normal, float penetration)
{
    this->position += normal * penetration; // Spostiamo la palla fuori dalla collisione
    this->direction = glm::reflect(this->direction, normal);
}

void Ball::changeDirection(glm::vec2 direction)
{
    this->activate();
    this->direction = direction;
}

void Ball::deactivate()
{
    this->active = false;
}

void Ball::activate()
{
    this->active = true;
}

bool Ball::isActive() const
{
    return this->active;
}

float Ball::getRadius()
{
    return this->radius;
}

glm::vec2 Ball::getPosition() const
{
    return this->position;
}

void Ball::changePosition(glm::vec2 newPosition)
{
    this->position = newPosition;
}
