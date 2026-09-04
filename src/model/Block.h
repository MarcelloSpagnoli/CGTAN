#ifndef BLOCK_H
#define BLOCK_H
#include <glm/glm.hpp>
#include "GridObject.h"

class Block : public GridObject
{
public:
    Block(GridObjectType type, glm::vec2 pos);

    int getHp() const;
    bool isActive() const override;
    void takeDamage(int damage);
    void initBlock(int hp);

private:
    int hp;
};
#endif