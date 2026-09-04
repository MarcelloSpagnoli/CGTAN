#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <glm/vec2.hpp>
#include "input.h"

class World;
class View;

enum class GameState
{
    StartMenu,
    Playing,
    GameOver
};

class Controller
{
public:
    Controller(World &world, View &view);
    void runFrame(float deltaTime, FrameInput &input);
    void showStartMenu();
    GameState getState() const;
    void writeRecord();
    int loadRecord();

private:
    World &world;
    View &view;
    GameState state = GameState::StartMenu;
    int record = 0;
};

#endif
