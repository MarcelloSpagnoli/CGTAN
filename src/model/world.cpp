#include "world.h"

#include <algorithm>
#include <iostream>
#include <thread>
#include "../configs.h"
using namespace std::chrono_literals;

World::World()
    : currentDamage(1), grid(COLS, ROWS, ROWS_FILL)
{
    for (int i = 0; i < INITIAL_BALLS; ++i)
    {
        balls.push_back(Ball(player.getPosition(), glm::vec2(0.0f, -1.0f)));
    }
    this->nextTurn = true;
}

bool World::update(float deltaTime)
{
    bool anyBallActive = false;

    // --- CONTROLLO ANTI-INCASTRAMENTO (Tempo scaduto) - FUORI DAL LOOP ---
    // Calcola UNA SOLA VOLTA se il timeout è scaduto
    bool timeoutTriggered = false;
    if (std::chrono::steady_clock::now() - this->turnStartTime > TIME_TOLERANCE)
    {
        timeoutTriggered = true;
        this->turnStartTime = std::chrono::steady_clock::now();
    }

    for (auto &ball : balls)
    {
        ball.update(deltaTime);
        if (!ball.isActive())
            continue;

        anyBallActive = true;

        // --- 1. COLLISIONI MURI ---
        CollisionInfo wallCollision = collisionDetector.checkBallWallCollision(ball, COLS / 2.0f, ROWS - 1.0f);
        if (wallCollision.collided)
        {
            ball.reflect(wallCollision.normal, wallCollision.penetration);
        }

        // --- 2. COLLISIONI BLOCCHI ---
        bool blockCollided = false;
        for (auto &block : grid.getBlocks())
        {
            if (block->getHp() > 0)
            {
                CollisionInfo info = collisionDetector.checkBallBlockCollision(ball, *block);
                if (info.collided)
                {
                    ball.reflect(info.normal, info.penetration);
                    block->takeDamage(currentDamage);
                    points += block->isActive() ? POINTS_PER_HIT : POINTS_PER_DESTROY;
                    blockCollided = true;
                    break;
                }
            }
        }

        // --- 3. COLLISIONI POTERI ---
        if (!blockCollided)
        {
            for (auto &power : grid.getPowers())
            {
                if (power->isActive())
                {
                    CollisionInfo info = collisionDetector.checkBallPowerCollision(ball, *power);

                    if (info.collided)
                    {
                        // Attiviamo solo se è un "nuovo" contatto per questa pallina
                        if (ball.getLastPower() != power.get())
                        {
                            ball.setLastPower(power.get()); // Memorizza il contatto

                            switch (power->getPowerType())
                            {
                            case PowerType::AddBall:
                                // Aggiungiamo alla lista temporanea
                                balls.push_back(Ball(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
                                power->deactivate(); // Questo può sparire subito
                                break;

                            case PowerType::LaserHorizontal:
                                for (auto &b : grid.getBlocks())
                                {
                                    if (b->getPosition().y == power->getPosition().y)
                                    {
                                        b->takeDamage(currentDamage);
                                        points += POINTS_PER_HIT;
                                    }
                                }
                                power->setLaserTime(); // Fa partire il flash grafico
                                break;

                            case PowerType::LaserVertical:
                                for (auto &b : grid.getBlocks())
                                {
                                    if (b->getPosition().x == power->getPosition().x)
                                    {
                                        b->takeDamage(currentDamage);
                                        points += POINTS_PER_HIT;
                                    }
                                }
                                power->setLaserTime(); // Fa partire il flash grafico
                                break;
                            }
                        }
                    }
                    else
                    {
                        // Se non c'è collisione, ma questa palla era "bloccata" su questo potere, resettiamo
                        if (ball.getLastPower() == power.get())
                        {
                            ball.setLastPower(nullptr);
                        }
                    }
                }
            }
        }

        // --- 3. APPLICA TIMEOUT A TUTTE LE PALLINE CONTEMPORANEAMENTE ---
        if (timeoutTriggered)
        {
            ball.changeDirection(glm::vec2(0.0f, -1.0f));
        }
    }

    // --- 5. FINE TURNO ---
    if (!anyBallActive && !nextTurn)
    {
        // Pulizia poteri residui prima del collapse
        for (auto &power : grid.getPowers())
        {
            power->deactivate();
        }
        grid.collapseDown();
        grid.spawnNewRow();
        grid.spawnPowers();
        nextTurn = true;
    }

    // --- 6. CONTROLLO GAME OVER (CON PROTEZIONE NULL POINTER) ---
    for (auto &block : grid.getBlocks())
    {
        if (block && block->isActive() && block->getPosition().y <= 2.0f)
        {
            return false; // Game Over
        }
    }
    return true;
}

void World::fireBalls(glm::vec2 direction)
{
    if (!nextTurn)
    {
        return; // Non permettere di sparare se le palline sono ancora in movimento
    }
    for (int i = 0; i < balls.size(); ++i)
    {
        balls[i].activate();
        glm::vec2 playerPos = player.getPosition() + glm::vec2(0.0f, 1.0f);
        balls[i].changePosition(playerPos);
        balls[i].changeDirection(direction);
        balls[i].setDelay(i * DELAY_BETWEEN_SHOTS); // Imposta un delay crescente per ogni palla
    }
    this->turnStartTime = std::chrono::steady_clock::now();
    nextTurn = false;
}

void World::movePlayer(float direction, float deltaTime)
{
    if (direction < -0.01f)
    {
        player.move(Direction::LEFT, deltaTime);
    }
    else if (direction > 0.01f)
    {
        player.move(Direction::RIGHT, deltaTime);
    }
}

const std::vector<Ball> &World::getBalls() const
{
    return balls;
}

const Player &World::getPlayer() const
{
    return player;
}

const Grid &World::getGrid() const
{
    return grid;
}

void World::worldClear()
{
    // Resetta il mondo allo stato iniziale
    balls.clear();
    for (int i = 0; i < INITIAL_BALLS; i++)
    {
        balls.push_back(Ball(glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
    }
    player = Player();
    grid = Grid(COLS, ROWS, ROWS_FILL);
    currentDamage = 1;
    points = 0;
    nextTurn = true;
}