#include "CollisionDetector.h"
#include "../configs.h"

CollisionDetector::CollisionDetector() {};

CollisionInfo CollisionDetector::checkBallBlockCollision(Ball &ball, Block &block)
{
    glm::vec2 ballPos = ball.getPosition();
    glm::vec2 closestPoint = getClosestPoint(ballPos, block.getPosition(), glm::vec2(BLOCK_SIZE * 0.5f));

    glm::vec2 diff = ballPos - closestPoint; // Vettore che punta verso l'esterno del blocco
    float distance = glm::dot(diff, diff);
    float radius = ball.getRadius();
    radius *= radius; // confronto con il quadrato del raggio per evitare sqrt

    if (distance < radius)
    {
        // Se la distanza è quasi zero (palla esattamente al centro dello spigolo),
        // evitiamo divisioni per zero usando una normale di default
        glm::vec2 normal = (distance > 0.0001f) ? glm::normalize(diff) : glm::vec2(0.0f, 1.0f);

        return {true, normal, radius - distance};
    }

    return {false, glm::vec2(0.0f), 0.0f};
}

CollisionInfo CollisionDetector::checkBallWallCollision(Ball &ball, float width, float height)
{
    glm::vec2 ballPos = ball.getPosition();
    float radius = ball.getRadius();

    // Parete Sinistra
    if (ballPos.x - radius < -width)
    {
        return {true, glm::vec2(1.0f, 0.0f), radius - (ballPos.x + width)};
    }
    // Parete Destra
    else if (ballPos.x + radius > width)
    {
        return {true, glm::vec2(-1.0f, 0.0f), (ballPos.x + radius) - width};
    }
    // Soffitto (Alto)
    if (ballPos.y + radius > height)
    {
        return {true, glm::vec2(0.0f, -1.0f), (ballPos.y + radius) - height};
    }

    // RIMOSSO il controllo ballPos.y - radius < 0.0f
    return {false, glm::vec2(0.0f), 0.0f};
}

CollisionInfo CollisionDetector::checkBallPowerCollision(Ball &ball, Power &power)
{
    glm::vec2 ballPos = ball.getPosition();
    glm::vec2 powerPos = power.getPosition();

    glm::vec2 diff = ballPos - powerPos;
    float distance = glm::dot(diff, diff);
    float radius = ball.getRadius() + POWER_RADIUS;
    radius *= radius; // confronto con il quadrato del raggio per evitare sqrt

    if (distance < radius)
    {
        return {true, glm::vec2(0.0f), radius - distance};
    }

    return {false, glm::vec2(0.0f), 0.0f};
}

glm::vec2 CollisionDetector::getClosestPoint(glm::vec2 ballPos, glm::vec2 blockPos, glm::vec2 blockHalfSize)
{
    // Calcoliamo i limiti del blocco
    glm::vec2 minBound = blockPos - blockHalfSize;
    glm::vec2 maxBound = blockPos + blockHalfSize;

    // clamping
    float closestX = glm::clamp(ballPos.x, minBound.x, maxBound.x);
    float closestY = glm::clamp(ballPos.y, minBound.y, maxBound.y);

    return glm::vec2(closestX, closestY);
}