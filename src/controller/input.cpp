#include "input.h"

#include <GLFW/glfw3.h>
#include <glm/geometric.hpp>
#include <iostream>
#include "../configs.h"

InputManager::InputManager(GLFWwindow *window) : window(window)
{
}

void InputManager::setWindow(GLFWwindow *window)
{
    this->window = window;
}

FrameInput InputManager::poll()
{
    FrameInput input;
    if (window == nullptr)
    {
        return input;
    }

    const bool leftPressed =
        glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
    const bool rightPressed =
        glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;

    if (leftPressed == rightPressed)
    {
        input.moveAxis = 0.0f;
    }
    else
    {
        input.moveAxis = rightPressed ? 1.0f : -1.0f;
    }

    input.shootPressed =
        glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    input.shootJustPressed = input.shootPressed && !previousShootPressed;
    previousShootPressed = input.shootPressed;

    const bool pausePressed =
        glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

    input.mouseWorldPos = computeMouseWorldPos();
    return input;
}

glm::vec2 InputManager::computeMouseWorldPos() const
{
    if (window == nullptr)
    {
        return glm::vec2(0.0f, -1.0f);
    }
    double mouseX = 0.0;
    double mouseY = 0.0;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    int fbWidth = 0;
    int fbHeight = 0;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    int winW = 0;
    int winH = 0;
    glfwGetWindowSize(window, &winW, &winH);

    if (winW <= 0 || winH <= 0)
    {
        return glm::vec2(0.0f, -1.0f);
    }

    // Converti da coordinate finestra a coordinate framebuffer
    float scaleX = static_cast<float>(fbWidth) / static_cast<float>(winW);
    float scaleY = static_cast<float>(fbHeight) / static_cast<float>(winH);

    float mouseFbX = static_cast<float>(mouseX) * scaleX;
    float mouseFbY = (static_cast<float>(winH) - static_cast<float>(mouseY)) * scaleY;

    float mouseNormX = (mouseFbX - this->vpX) / this->vpW;
    float mouseNormY = (mouseFbY - this->vpY) / this->vpH;

    // Converti da coordinate normalizzate a coordinate mondo
    // Assumendo aspect ratio 9:14
    float worldX = (mouseNormX - 0.5f) * (COLS / 2.0f) * 2.0f;
    float worldY = (mouseNormY)*ROWS;

    return glm::vec2(worldX, worldY);
}

void InputManager::setViewportInfo(std::tuple<float, float, float, float> viewportInfo)
{
    const auto [vpX, vpY, vpW, vpH] = viewportInfo;
    this->vpX = vpX;
    this->vpY = vpY;
    this->vpW = vpW;
    this->vpH = vpH;
}

void InputManager::resetInput()
{
    previousShootPressed = false;
}
