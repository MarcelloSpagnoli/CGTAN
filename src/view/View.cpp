#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_NONE

#include "View.h"
#include "../model/world.h"
#include "../model/Block.h"
#include "../model/Power.h"
#include "../configs.h"
#include "RenderCommand.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <type_traits>
#include <stdexcept>
#include <iostream>

void View::initView()
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE); // db buffering

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    this->width = static_cast<int>(mode->width * 0.8f);
    this->height = static_cast<int>(mode->height * 0.8f);
    this->window = glfwCreateWindow(width, height, TITLE, nullptr, nullptr);

    if (!this->window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwDestroyWindow(window);
        window = nullptr;
        glfwTerminate();
        throw std::runtime_error("Failed to load OpenGL functions");
    }
    initShaders();
    initGeometry();
    this->gui.init(window);

    textrenderer = new TextRenderer(ASSETS_DIR "/font/font.ttf", 24);

    glfwSwapInterval(1);
}

UiCommand View::render(GameState gameState, unsigned int score)
{
    if (window == nullptr)
        return UiCommand::None;

    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

    const float targetAspect = COLS / static_cast<float>(ROWS);
    const float fbAspect = static_cast<float>(fbWidth) / static_cast<float>(fbHeight);

    vpW = fbWidth;
    vpH = fbHeight;

    if (fbAspect > targetAspect)
    {
        vpH = fbHeight;
        vpW = static_cast<int>(vpH * targetAspect);
        vpX = (fbWidth - vpW) / 2;
        vpY = 0;
    }
    else
    {
        vpW = fbWidth;
        vpH = static_cast<int>(vpW / targetAspect);
        vpX = 0;
        vpY = (fbHeight - vpH) / 2;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(vpX, vpY, vpW, vpH);
    renderBackground(vpW, vpH, vpX, vpY);
    executeRenderCommands();
    renderTextQueue();

    int winW = 0, winH = 0;
    glfwGetWindowSize(window, &winW, &winH);
    const float xscale = winW > 0 ? static_cast<float>(fbWidth) / static_cast<float>(winW) : 1.0f;
    const float yscale = winH > 0 ? static_cast<float>(fbHeight) / static_cast<float>(winH) : 1.0f;

    // Convertiamo i parametri del viewport in unità logiche per ImGui
    // (usiamo il rapporto framebuffer/finestra misurato, non glfwGetWindowContentScale,
    // così restano coerenti col mapping del mouse in renderBackground)
    float guiX = vpX / xscale;
    float guiY = vpY / yscale;
    float guiW = vpW / xscale;
    float guiH = vpH / yscale;

    UiCommand command = UiCommand::None;
    this->gui.beginFrame();

    if (gameState == GameState::StartMenu || gameState == GameState::GameOver)
    {
        // Passiamo i valori normalizzati
        command = this->gui.drawMenu(score, guiX, guiY, guiW, guiH);
    }
    else if (gameState == GameState::Playing)
    {
        // Passiamo i valori normalizzati
        command = this->gui.drawHud(score, guiX, guiY, guiW, guiH);
    }

    this->gui.endFrame();

    if (command == UiCommand::Exit)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    glfwSwapBuffers(window);
    return command;
}

void View::shutdownView()
{
    for (auto &p : vbos)
        glDeleteBuffers(1, &p.second);
    for (auto &p : vaos)
        glDeleteVertexArrays(1, &p.second);
    vbos.clear();
    vaos.clear();
    this->gui.shutdown();
    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void View::pollEvents() const
{
    glfwPollEvents();
    if (window != nullptr && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

bool View::shouldClose() const
{
    return window == nullptr || glfwWindowShouldClose(window);
}

void View::initShaders()
{
    shaderPrograms.clear();

    ShaderProgram background;
    background.loadFromFiles(
        ASSETS_DIR "/shaders/background.vert",
        ASSETS_DIR "/shaders/background.frag");

    shaderPrograms.emplace("background", std::move(background));

    ShaderProgram object;
    object.loadFromFiles(
        ASSETS_DIR "/shaders/object.vert",
        ASSETS_DIR "/shaders/object.frag");
    shaderPrograms.emplace("object", std::move(object));

    ShaderProgram laser;
    laser.loadFromFiles(
        ASSETS_DIR "/shaders/laser.vert",
        ASSETS_DIR "/shaders/laser.frag");
    shaderPrograms.emplace("laser", std::move(laser));

    ShaderProgram hermite;
    hermite.loadFromFiles(
        ASSETS_DIR "/shaders/hermite.vert",
        ASSETS_DIR "/shaders/hermite.frag");
    shaderPrograms.emplace("hermite", std::move(hermite));
}

void View::buildRenderQueue(const World &world)
{
    renderQueue.clear();
    textQueue.clear();
    glm::mat4 proj = glm::ortho(-COLS / 2.0f, COLS / 2.0f, 0.0f, ROWS, -1.0f, 1.0f);

    // --- Render BLOCCHI ---
    for (const auto &block : world.getGrid().getBlocks())
    {
        if (block->getHp() <= 0)
            continue;

        RenderCommand blockCommand;
        blockCommand.shaderName = "object";
        blockCommand.geometryName = "block";
        blockCommand.vertexCount = 6;
        blockCommand.primitive = GL_TRIANGLES;
        blockCommand.uniforms.emplace("uIsBlock", RenderCommand::RenderCommand::makeUniformValue(true));

        glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                         glm::vec3(block->getPosition().x, block->getPosition().y, 0.0f));
        blockCommand.uniforms.emplace("uModel", RenderCommand::makeUniformValue(model));

        glm::mat4 scale = glm::scale(glm::mat4(1.0f),
                                     glm::vec3(BLOCK_SIZE * 0.95f, BLOCK_SIZE * 0.95f, 1.0f));
        blockCommand.uniforms.emplace("uScale", RenderCommand::makeUniformValue(scale));

        blockCommand.uniforms.emplace("uProjection", RenderCommand::makeUniformValue(proj));
        blockCommand.uniforms.emplace("uBorderThickness", RenderCommand::makeUniformValue(BORDER_THICKNESS));

        // Colori
        glm::vec3 colorIntegro(0.95f, 0.35f, 0.35f);
        glm::vec3 colorQuasiRotto(0.95f, 0.75f, 0.20f);
        float currentHp = static_cast<float>(block->getHp());
        float maxHp = MAX_HP;
        float t = 0.0f;
        if (maxHp > 1.0f)
        {
            t = glm::clamp((maxHp - currentHp) / (maxHp - 1.0f), 0.0f, 1.0f);
        }
        glm::vec3 finalColor = glm::mix(colorIntegro, colorQuasiRotto, t);
        blockCommand.uniforms.emplace("uColor", RenderCommand::makeUniformValue(finalColor));

        renderQueue.push_back(std::move(blockCommand));
        textQueue.push_back({std::to_string(block->getHp()), block->getPosition().x, block->getPosition().y, finalColor});
    }

    // --- Render POTERI ---
    for (const auto &power : world.getGrid().getPowers())
    {
        if (!power->isActive())
            continue;

        RenderCommand powerCommand;
        powerCommand.shaderName = "object";
        powerCommand.geometryName = "circle";
        powerCommand.vertexCount = CIRCLE_SEGMENTS + 2;
        powerCommand.primitive = GL_TRIANGLE_FAN;
        powerCommand.uniforms.emplace("uIsBlock", RenderCommand::makeUniformValue(false));

        // CORRETTO: uModel come matrice
        glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                         glm::vec3(power->getPosition().x, power->getPosition().y, 0.0f));
        powerCommand.uniforms.emplace("uModel", RenderCommand::makeUniformValue(model));

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        powerCommand.uniforms.emplace("uScale", RenderCommand::makeUniformValue(scale));

        powerCommand.uniforms.emplace("uProjection", RenderCommand::makeUniformValue(proj));

        switch (power->getPowerType())
        {
        case PowerType::AddBall:
            powerCommand.uniforms.emplace("uColor", RenderCommand::makeUniformValue(glm::vec3(0.95f, 0.20f, 0.95f)));
            break;
        case PowerType::LaserHorizontal:
        {
            glm::vec3 laserColor = glm::vec3(0.0f, 0.788f, 0.341f);
            powerCommand.uniforms.emplace("uColor", RenderCommand::makeUniformValue(laserColor));
            auto elapsed = std::chrono::steady_clock::now() - power->getLaserTime();
            float timeMs = std::chrono::duration<float, std::milli>(elapsed).count();
            if (timeMs < LASER_TIME_MILLIS)
            {
                RenderCommand laserCommand;
                laserCommand.shaderName = "laser";
                laserCommand.geometryName = "horizontal_laser";
                laserCommand.vertexCount = 6;
                laserCommand.primitive = GL_TRIANGLES;
                laserCommand.uniforms.emplace("uTimeSinceHit", RenderCommand::makeUniformValue(timeMs));
                laserCommand.uniforms.emplace("uMaxLife", RenderCommand::makeUniformValue((float)LASER_TIME_MILLIS));
                laserCommand.uniforms.emplace("uColor", RenderCommand::makeUniformValue(laserColor));
                laserCommand.uniforms.emplace("rows", RenderCommand::makeUniformValue(ROWS - 1));
                laserCommand.uniforms.emplace("uModel", RenderCommand::makeUniformValue(power->getPosition()));
                laserCommand.uniforms.emplace("uProjection", RenderCommand::makeUniformValue(proj));
                laserCommand.uniforms.emplace("uScale", RenderCommand::makeUniformValue(glm::vec2(COLS, 1.0f)));
                renderQueue.push_back(std::move(laserCommand));
            }
            break;
        }
        case PowerType::LaserVertical:
        {
            glm::vec3 laserColor = glm::vec3(0.95f, 0.95f, 0.20f);
            powerCommand.uniforms.emplace("uColor", RenderCommand::makeUniformValue(laserColor));
            auto elapsed = std::chrono::steady_clock::now() - power->getLaserTime();
            float timeMs = std::chrono::duration<float, std::milli>(elapsed).count();
            if (timeMs < LASER_TIME_MILLIS)
            {
                RenderCommand laserCommand;
                laserCommand.shaderName = "laser";
                laserCommand.geometryName = "vertical_laser";
                laserCommand.vertexCount = 6;
                laserCommand.primitive = GL_TRIANGLES;
                laserCommand.uniforms.emplace("uTimeSinceHit", RenderCommand::makeUniformValue(timeMs));
                laserCommand.uniforms.emplace("uMaxLife", RenderCommand::makeUniformValue((float)LASER_TIME_MILLIS));
                laserCommand.uniforms.emplace("uColor", RenderCommand::makeUniformValue(laserColor));
                laserCommand.uniforms.emplace("rows", RenderCommand::makeUniformValue(ROWS - 1));
                laserCommand.uniforms.emplace("uModel", RenderCommand::makeUniformValue(power->getPosition()));
                laserCommand.uniforms.emplace("uScale", RenderCommand::makeUniformValue(glm::vec2(1.0f, ROWS - 1)));
                laserCommand.uniforms.emplace("uProjection", RenderCommand::makeUniformValue(proj));
                renderQueue.push_back(std::move(laserCommand));
            }
            break;
        }
        default:
            break;
        }
        renderQueue.push_back(std::move(powerCommand));
    }

    // --- Render PALLINE ---
    for (const auto &ball : world.getBalls())
    {
        if (!ball.isActive())
            continue;

        RenderCommand projectileCommand;
        projectileCommand.shaderName = "object";
        projectileCommand.geometryName = "circle";
        projectileCommand.vertexCount = CIRCLE_SEGMENTS + 2;
        projectileCommand.primitive = GL_TRIANGLE_FAN;
        projectileCommand.uniforms.emplace("uIsBlock", RenderCommand::makeUniformValue(false));

        // CORRETTO: uModel come matrice
        glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                         glm::vec3(ball.getPosition().x, ball.getPosition().y, 0.0f));
        projectileCommand.uniforms.emplace("uModel", RenderCommand::makeUniformValue(model));

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        projectileCommand.uniforms.emplace("uScale", RenderCommand::makeUniformValue(scale));

        projectileCommand.uniforms.emplace("uProjection", RenderCommand::makeUniformValue(proj));
        projectileCommand.uniforms.emplace("uColor", RenderCommand::makeUniformValue(glm::vec3(1.0f, 1.0f, 1.0f)));

        renderQueue.push_back(std::move(projectileCommand));
    }

    // --- Render PLAYER ---
    const Player &player = world.getPlayer();
    const glm::vec2 playerPos = player.getPosition() + glm::vec2(0.0f, 1.0f);

    RenderCommand playerCommand;
    playerCommand.shaderName = "hermite";
    playerCommand.geometryName = "player";
    playerCommand.vertexCount = playerVertexCount;
    playerCommand.primitive = GL_TRIANGLES;

    // CORRETTO: uModel come matrice
    glm::mat4 playerModel = glm::translate(glm::mat4(1.0f),
                                           glm::vec3(playerPos.x, playerPos.y, 0.0f));
    playerCommand.uniforms.emplace("uModel", RenderCommand::makeUniformValue(playerModel));

    glm::mat4 playerScale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    playerCommand.uniforms.emplace("uScale", RenderCommand::makeUniformValue(playerScale));

    playerCommand.uniforms.emplace("uProjection", RenderCommand::makeUniformValue(proj));
    playerCommand.uniforms.emplace("uUseSolidColor", RenderCommand::makeUniformValue(false));
    textQueue.push_back({"x" + std::to_string(world.getNumBalls()), player.getPosition().x, player.getPosition().y + 0.6f, glm::vec3(1.0f, 1.0f, 1.0f)});
    renderQueue.push_back(std::move(playerCommand));
}

void View::renderBackground(float vpW, float vpH, float vpX, float vpY)
{
    const auto shaderIt = shaderPrograms.find("background");
    if (shaderIt == shaderPrograms.end())
        throw std::runtime_error("Missing shader: background");

    const auto vaoIt = vaos.find("background");
    if (vaoIt == vaos.end())
        throw std::runtime_error("Missing geometry: background");

    shaderIt->second.use();
    glUniform1f(shaderIt->second.uniformLocation("uTime"), glfwGetTime());
    glUniform1f(shaderIt->second.uniformLocation("rows"), ROWS);
    glUniform2f(shaderIt->second.uniformLocation("uResolution"), static_cast<float>(vpW), static_cast<float>(vpH));
    glUniform2f(shaderIt->second.uniformLocation("uViewportOffset"), static_cast<float>(vpX), static_cast<float>(vpY));

    double mouseX = 0.0, mouseY = 0.0;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    int winW = 0, winH = 0;
    glfwGetWindowSize(window, &winW, &winH);

    float mouseNormX = 0.5f, mouseNormY = 0.5f;
    if (winW > 0 && winH > 0)
    {
        const float scaleX = static_cast<float>(fbWidth) / static_cast<float>(winW);
        const float scaleY = static_cast<float>(fbHeight) / static_cast<float>(winH);
        const float mouseFbX = static_cast<float>(mouseX) * scaleX;
        const float mouseFbY = (static_cast<float>(winH) - static_cast<float>(mouseY)) * scaleY;
        mouseNormX = (mouseFbX - static_cast<float>(vpX)) / static_cast<float>(vpW);
        mouseNormY = (mouseFbY - static_cast<float>(vpY)) / static_cast<float>(vpH);
    }
    glUniform2f(shaderIt->second.uniformLocation("uMouse"), mouseNormX, mouseNormY);

    glBindVertexArray(vaoIt->second);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void View::executeRenderCommands()
{
    for (const RenderCommand &command : renderQueue)
    {
        const auto objectShaderIt = shaderPrograms.find(command.shaderName);
        if (objectShaderIt == shaderPrograms.end())
            throw std::runtime_error("Missing shader: " + command.shaderName);

        const auto objectVaoIt = vaos.find(command.geometryName);
        if (objectVaoIt == vaos.end())
            throw std::runtime_error("Missing geometry: " + command.geometryName);

        objectShaderIt->second.use();
        for (const auto &[uniformName, uniformValue] : command.uniforms)
        {
            const int location = objectShaderIt->second.uniformLocation(uniformName.c_str());
            if (location < 0)
                continue;
            std::visit(
                [location](const auto &uniformValue)
                {
                    using T = std::decay_t<decltype(uniformValue)>;
                    if constexpr (std::is_same_v<T, float>)
                        glUniform1f(location, uniformValue);
                    else if constexpr (std::is_same_v<T, bool>)
                        glUniform1i(location, uniformValue ? 1 : 0);
                    else if constexpr (std::is_same_v<T, int>)
                        glUniform1i(location, uniformValue);
                    else if constexpr (std::is_same_v<T, unsigned int>)
                        glUniform1ui(location, uniformValue);
                    else if constexpr (std::is_same_v<T, glm::vec2>)
                        glUniform2f(location, uniformValue.x, uniformValue.y);
                    else if constexpr (std::is_same_v<T, glm::vec3>)
                        glUniform3f(location, uniformValue.x, uniformValue.y, uniformValue.z);
                    else if constexpr (std::is_same_v<T, glm::vec4>)
                        glUniform4f(location, uniformValue.x, uniformValue.y, uniformValue.z, uniformValue.w);
                    else if constexpr (std::is_same_v<T, glm::mat3>)
                        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(uniformValue));
                    else if constexpr (std::is_same_v<T, glm::mat4>)
                        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(uniformValue));
                },
                uniformValue);
        }

        if (command.shaderName == "hermite")
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        }
        else
        {
            glDisable(GL_BLEND);
        }

        if (command.primitive == GL_LINES || command.primitive == GL_LINE_STRIP || command.primitive == GL_LINE_LOOP)
            glLineWidth(command.lineWidth);

        glBindVertexArray(objectVaoIt->second);
        glDrawArrays(command.primitive, 0, command.vertexCount);
        glBindVertexArray(0);

        if (command.primitive == GL_LINES || command.primitive == GL_LINE_STRIP || command.primitive == GL_LINE_LOOP)
            glLineWidth(1.0f);
    }
    glDisable(GL_BLEND);
}

void View::renderTextQueue()
{
    textrenderer->setProjection(glm::ortho(-COLS / 2.0f, COLS / 2.0f, 0.0f, ROWS, -1.0f, 1.0f));
    textrenderer->beginBatch();
    for (const auto &text : textQueue)
    {
        textrenderer->addText(text.text, text.x, text.y, text.color);
    }
    textrenderer->renderBatch();
}

glm::vec3 View::hermite(glm::vec3 p0, glm::vec3 p1, glm::vec3 t0, glm::vec3 t1, float u)
{
    float h1 = 2.0f * u * u * u - 3.0f * u * u + 1.0f;
    float h2 = -2.0f * u * u * u + 3.0f * u * u;
    float h3 = u * u * u - 2.0f * u * u + u;
    float h4 = u * u * u - u * u;
    return h1 * p0 + h2 * p1 + h3 * t0 + h4 * t1;
}
