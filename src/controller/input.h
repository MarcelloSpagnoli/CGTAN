#ifndef CONTROLLER_INPUT_H
#define CONTROLLER_INPUT_H

#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>
#include <tuple>

struct FrameInput
{
    float moveAxis = 0.0f;
    bool shootPressed = false;
    bool shootJustPressed = false;
    glm::vec2 mouseWorldPos{0.0f, 0.0f};
};

class InputManager
{
public:
    explicit InputManager(GLFWwindow *window);
    void setWindow(GLFWwindow *window);
    FrameInput poll();
    void setViewportInfo(std::tuple<float, float, float, float> viewportInfo);
    void resetInput(); // Reset state between game transitions

private:
    glm::vec2 computeMouseWorldPos() const;
    float vpX, vpY, vpW, vpH;
    GLFWwindow *window = nullptr;
    bool previousShootPressed = false;
};

#endif
