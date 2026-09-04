#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>

enum class Direction
{
    LEFT,
    RIGHT
};

class Player
{
public:
    Player();
    float width = 1.0f;
    float height = 2.0f;
    void move(Direction direction, float deltaTime);
    glm::vec2 getPosition() const;

private:
    glm::vec2 position;
};

#endif
