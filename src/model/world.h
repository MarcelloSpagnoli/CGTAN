#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <glm/glm.hpp>
#include "Ball.h"
#include "Block.h"
#include "Player.h"
#include "Grid.h"
#include "CollisionDetector.h"
#include <chrono> // Libreria fondamentale

class World
{
public:
    World();
    bool update(float deltaTime);
    void fireBalls(glm::vec2 direction);
    void movePlayer(float direction, float deltaTime);
    const std::vector<Ball> &getBalls() const;
    const Player &getPlayer() const;
    const Grid &getGrid() const;
    unsigned int getPoints() const { return points; }
    void worldClear();
    int getNumBalls() const { return static_cast<int>(balls.size()); }

private:
    int currentDamage;
    std::vector<Ball> balls;
    Player player;
    Grid grid;
    CollisionDetector collisionDetector;
    bool nextTurn;
    std::chrono::steady_clock::time_point turnStartTime;
    unsigned int points = 0;
};

#endif
