#ifndef GRID_OBJECT_H
#define GRID_OBJECT_H

#include <glm/glm.hpp>

enum class GridObjectType
{
    Power,
    Block
};

class GridObject
{
public:
    GridObject(GridObjectType type, glm::vec2 position);
    virtual ~GridObject() = default;

    GridObjectType getType() const;
    glm::vec2 getPosition() const;
    virtual bool isActive() const = 0;
    virtual void setPosition(glm::vec2 newPos)
    {
        position = newPos;
    }

protected:
    GridObjectType type;
    glm::vec2 position;
};

#endif