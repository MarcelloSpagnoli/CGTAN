#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include <glm/glm.hpp>
#include "Ball.h"
#include "Block.h"
#include "Power.h"

struct CollisionInfo
{
    bool collided;
    glm::vec2 normal;
    float penetration;
};

class CollisionDetector
{
public:
    CollisionDetector();
    CollisionInfo checkBallBlockCollision(Ball &ball, Block &block);
    CollisionInfo checkBallWallCollision(Ball &ball, float width, float height);
    CollisionInfo checkBallPowerCollision(Ball &ball, Power &power);

private:
    glm::vec2 getClosestPoint(glm::vec2 ballPos, glm::vec2 blockPos, glm::vec2 blockHalfSize);
};

#endif
