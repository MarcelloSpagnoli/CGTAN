#include "GridObject.h"

GridObject::GridObject(GridObjectType type, glm::vec2 position)
    : type(type), position(position)
{
}

GridObjectType GridObject::getType() const
{
    return type;
}

glm::vec2 GridObject::getPosition() const
{
    return position;
}