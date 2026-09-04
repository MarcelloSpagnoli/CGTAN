#include "Player.h"
#include "../configs.h"

Player::Player() : position(0.0f, 0.0f) {}

void Player::move(Direction direction, float deltaTime)
{
    switch (direction)
    {
    case Direction::LEFT:
        this->position.x -= deltaTime * PLAYER_SPEED;
        break;
    case Direction::RIGHT:
        this->position.x += deltaTime * PLAYER_SPEED;
        break;
    }
    if (this->position.x < -COLS / 2.0f)
        this->position.x = -COLS / 2.0f;
    if (this->position.x > COLS / 2.0f)
        this->position.x = COLS / 2.0f;
}

glm::vec2 Player::getPosition() const
{
    return this->position;
}
