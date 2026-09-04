#include "Grid.h"
#include "Block.h"
#include "Power.h"
#include <random>
#include <algorithm>

Grid::Grid(int width, int height, int rowsFill)
    : width(width), height(height)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dis(SPAWN_PROBABILITY);

    // Distribuzione float per le soglie dei tipi (0.0 - 10.0)
    std::uniform_real_distribution<float> typeDis(0.0f, 10.0f);
    // Distribuzione per HP casuali tra 1 e un massimo (es. 5 o MAX_HP)
    std::uniform_int_distribution<> hpDis(1, static_cast<int>(MAX_HP));

    for (int y = height - 2; y >= rowsFill; --y)
    {
        for (int x = -width / 2; x < width / 2; ++x)
        {
            if (dis(gen))
            {
                glm::vec2 pos = glm::vec2(x * BLOCK_SIZE + BLOCK_SIZE / 2.0f,
                                          y * BLOCK_SIZE + BLOCK_SIZE / 2.0f);

                float typeRand = typeDis(gen);

                if (typeRand < 8.5f) // 85% Probabilità Blocco
                {
                    auto block = std::make_shared<Block>(GridObjectType::Block, pos);
                    block->initBlock(hpDis(gen)); // HP Casuali al setup iniziale
                    grid.push_back(block);
                    blocks.push_back(block);
                }
                else // 15% Probabilità Poteri
                {
                    auto power = std::make_shared<Power>(GridObjectType::Power, pos);
                    if (typeRand < 9.0f)
                        power->initPower(PowerType::LaserVertical);
                    else if (typeRand < 9.5f)
                        power->initPower(PowerType::LaserHorizontal);
                    else
                        power->initPower(PowerType::AddBall);

                    grid.push_back(power);
                    powers.push_back(power);
                }
            }
        }
    }
}

std::vector<std::shared_ptr<GridObject>> &Grid::getGrid()
{
    return grid;
}

const std::vector<std::shared_ptr<GridObject>> &Grid::getGrid() const
{
    return grid;
}

void Grid::collapseDown()
{
    for (auto &obj : grid)
    {
        glm::vec2 position = obj->getPosition();
        position.y -= BLOCK_SIZE;
        obj->setPosition(position);
    }
}

void Grid::spawnNewRow()
{
    // Pulizia preventiva dei blocchi distrutti
    deleteDestroyedBlocks();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dis(SPAWN_PROBABILITY);
    std::uniform_real_distribution<float> typeDis(0.0f, 10.0f);
    std::uniform_int_distribution<> hpDis(1, static_cast<int>(MAX_HP));

    for (int x = -width / 2; x < width / 2; ++x)
    {
        if (dis(gen))
        {
            glm::vec2 pos = glm::vec2(x * BLOCK_SIZE + BLOCK_SIZE / 2.0f,
                                      (height - 2) * BLOCK_SIZE + BLOCK_SIZE / 2.0f);

            float typeRand = typeDis(gen);

            if (typeRand < 9.0f) // Nella nuova riga favoriamo i blocchi (90%)
            {
                auto block = std::make_shared<Block>(GridObjectType::Block, pos);
                block->initBlock(hpDis(gen));
                grid.push_back(block);
                blocks.push_back(block);
            }
            else // 10% possibilità di un AddBall direttamente in nuova riga
            {
                auto power = std::make_shared<Power>(GridObjectType::Power, pos);
                power->initPower(PowerType::AddBall);
                grid.push_back(power);
                powers.push_back(power);
            }
        }
    }
}

void Grid::spawnPowers()
{
    std::random_device rd;
    std::mt19937 gen(rd());

    // Probabilità di spawn potere extra per ogni slot libero
    std::bernoulli_distribution chanceDis(0.2f); // 10% di successo
    std::uniform_real_distribution<float> typeDis(0.0f, 10.0f);

    std::vector<glm::vec2> freeSlots;

    // Scansioniamo la griglia (escludendo la zona troppo vicina al giocatore, y < 3)
    for (int y = 3; y < height - 2; ++y)
    {
        for (int x = -width / 2; x < width / 2; ++x)
        {
            glm::vec2 targetPos = glm::vec2(x * BLOCK_SIZE + BLOCK_SIZE / 2.0f,
                                            y * BLOCK_SIZE + BLOCK_SIZE / 2.0f);

            bool occupied = std::any_of(grid.begin(), grid.end(), [&](const std::shared_ptr<GridObject> &obj)
                                        { return glm::distance(obj->getPosition(), targetPos) < 0.1f; });

            if (!occupied)
            {
                freeSlots.size();
                freeSlots.push_back(targetPos);
            }
        }
    }

    // Se abbiamo slot liberi, proviamo a spawnare
    if (!freeSlots.empty())
    {
        // Mescoliamo gli slot liberi per non spawnare sempre negli stessi posti
        std::shuffle(freeSlots.begin(), freeSlots.end(), gen);

        for (const auto &pos : freeSlots)
        {
            if (chanceDis(gen)) // Lancio per ogni slot libero
            {
                float typeRand = typeDis(gen);
                auto power = std::make_shared<Power>(GridObjectType::Power, pos);

                if (typeRand < 3.3f)
                    power->initPower(PowerType::LaserVertical);
                else if (typeRand < 6.6f)
                    power->initPower(PowerType::LaserHorizontal);
                else
                    power->initPower(PowerType::AddBall);

                grid.push_back(power);
                powers.push_back(power);

                // Limitiamo a un solo potere extra per turno per non esagerare
                break;
            }
        }
    }
}

void Grid::deleteDestroyedBlocks()
{
    grid.erase(
        std::remove_if(grid.begin(), grid.end(),
                       [](const std::shared_ptr<GridObject> &obj)
                       {
                           return !obj->isActive();
                       }),
        grid.end());

    blocks.erase(
        std::remove_if(blocks.begin(), blocks.end(),
                       [](const std::shared_ptr<Block> &block)
                       {
                           return !block->isActive();
                       }),
        blocks.end());

    powers.erase(
        std::remove_if(powers.begin(), powers.end(),
                       [](const std::shared_ptr<Power> &power)
                       {
                           return !power->isActive();
                       }),
        powers.end());
}