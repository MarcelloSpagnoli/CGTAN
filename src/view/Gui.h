#ifndef GUI_H
#define GUI_H

#include "imgui.h"
#include "glm/glm.hpp"
#include <string>

struct GLFWwindow;
class World;

enum class UiCommand
{
    None,
    StartGame,
    Exit,
    BackToMenu
};

class Gui
{
public:
    void init(GLFWwindow *window);
    void shutdown();
    void beginFrame();
    UiCommand drawHud(const int points, float vpX, float vpY, float vpW, float vpH);
    UiCommand drawMenu(int points, float vpX, float vpY, float vpW, float vpH);
    void endFrame();

private:
    void writePoints(const std::string &text, int points, float x, float y);
    GLFWwindow *window = nullptr;
    ImFont *font = nullptr;
    ImFont *titleFont = nullptr;
    glm::vec4 pointsColors[8] = {

        glm::vec4(0.2f, 1.0f, 0.0f, 1.0f),

        glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),

        glm::vec4(0.0f, 0.8f, 1.0f, 1.0f),

        glm::vec4(1.0f, 0.5f, 0.0f, 1.0f),

        glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),

        glm::vec4(0.0f, 0.4f, 1.0f, 1.0f),

        glm::vec4(1.0f, 0.1f, 0.1f, 1.0f),

        glm::vec4(0.6f, 0.0f, 1.0f, 1.0f)};
};

#endif