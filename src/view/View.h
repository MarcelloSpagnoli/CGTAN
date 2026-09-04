#ifndef RENDERING_H
#define RENDERING_H

#include "RenderCommand.h"
#include "shaders/Shader.h"
#include <string>
#include <unordered_map>
#include <vector>
#include "Gui.h"
#include "glm/glm.hpp"
#include "TextRenderer.h"
#include "../controller/controller.h"

struct GLFWwindow;

class World;

struct TextToDraw
{
    std::string text;
    float x, y;
    glm::vec3 color;
};

class View
{
public:
    UiCommand render(GameState gameState, unsigned int score = 0);
    void buildRenderQueue(const World &world);
    void clearRenderQueue() { renderQueue.clear(); }
    void clearTextQueue() { textQueue.clear(); }
    void initView();
    void shutdownView();
    bool shouldClose() const;
    void pollEvents() const;
    ~View() = default;
    GLFWwindow *getWindow() const { return window; }
    std::tuple<float, float, float, float> getViewPortInfo() const { return std::make_tuple(vpX, vpY, vpW, vpH); }

private:
    GLFWwindow *window = nullptr;
    int width = 0;
    int height = 0;

    // Shaders
    std::unordered_map<std::string, ShaderProgram> shaderPrograms;

    // Geometry
    std::unordered_map<std::string, unsigned int> vaos;
    std::unordered_map<std::string, unsigned int> vbos;

    // Initialization
    void initShaders();
    void initGeometry();
    void createGeometry(const std::string &name, const std::vector<float> &vertices, int componentsPerVertex);

    // Rendering helpers
    void renderBackground(float vpW, float vpH, float vpX, float vpY);
    void executeRenderCommands();
    void renderTextQueue();

    // Interpolation
    glm::vec3 hermite(glm::vec3 p0, glm::vec3 p1, glm::vec3 t0, glm::vec3 t1, float u);

    // UI & Rendering state
    Gui gui;
    std::vector<RenderCommand> renderQueue;
    std::vector<TextToDraw> textQueue;
    int playerVertexCount = 0;
    int fbWidth, fbHeight;
    float vpX = 0, vpY = 0, vpW = 0, vpH = 0;
    TextRenderer *textrenderer;
};

#endif
