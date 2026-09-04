#ifndef GRID_H
#define GRID_H
#include "GridObject.h"
#include "../configs.h"
#include <memory>
#include <vector>

class Block;
class Power;

class Grid
{
public:
    Grid(int width, int height, int rowsFill);

    std::vector<std::shared_ptr<GridObject>> &getGrid();
    const std::vector<std::shared_ptr<GridObject>> &getGrid() const;
    
    std::vector<std::shared_ptr<Block>> &getBlocks() { return blocks; }
    const std::vector<std::shared_ptr<Block>> &getBlocks() const { return blocks; }
    
    std::vector<std::shared_ptr<Power>> &getPowers() { return powers; }
    const std::vector<std::shared_ptr<Power>> &getPowers() const { return powers; }

    void spawnNewRow();
    void collapseDown();
    void deleteDestroyedBlocks();
    void spawnPowers();

private:
    std::vector<std::shared_ptr<GridObject>> grid;
    std::vector<std::shared_ptr<Block>> blocks;
    std::vector<std::shared_ptr<Power>> powers;
    int width;
    int height;
};
#endif