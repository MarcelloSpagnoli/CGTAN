#include "Block.h"

Block::Block(GridObjectType type, glm::vec2 pos)
    : GridObject(type, pos), hp(0)
{
}

int Block::getHp() const
{
    return hp;
}

bool Block::isActive() const
{
    return hp > 0;
}

void Block::takeDamage(int damage)
{
    hp -= damage;
}

void Block::initBlock(int hp)
{
    this->hp = hp;
}