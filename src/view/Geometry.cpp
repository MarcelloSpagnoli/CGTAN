#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_NONE

#include "View.h"
#include "../configs.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void View::initGeometry()
{
    vaos.clear();
    vbos.clear();

    const std::vector<float> backgroundVertices = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,

        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f};

    createGeometry("background", backgroundVertices, 3);

    const std::vector<float> quadVertices = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,

        -0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f};
    createGeometry("block", quadVertices, 3);

    unsigned int segmentCount = CIRCLE_SEGMENTS;

    const std::vector<float> VerticaLaserVertices = {
        -0.05f, -1.0f, 0.0f,
        0.05f, -1.0f, 0.0f,
        0.05f, 1.0f, 0.0f,

        -0.05f, -1.0f, 0.0f,
        0.05f, 1.0f, 0.0f,
        -0.05f, 1.0f, 0.0f};

    createGeometry("vertical_laser", VerticaLaserVertices, 3);

    const std::vector<float> HorizontasLaserVertices = {
        -1.0f, -0.05f, 0.0f,
        1.0f, -0.05f, 0.0f,
        1.0f, 0.05f, 0.0f,

        -1.0f, -0.05f, 0.0f,
        1.0f, 0.05f, 0.0f,
        -1.0f, 0.05f, 0.0f};

    createGeometry("horizontal_laser", HorizontasLaserVertices, 3);

    std::vector<float> circleVertices;
    float centerX = 0.0f;
    float centerY = 0.0f;
    float radius = 0.1f;
    // 1. Aggiungi il centro (fondamentale per il FAN)
    circleVertices.push_back(centerX);
    circleVertices.push_back(centerY);
    circleVertices.push_back(0.0f);

    for (unsigned int i = 0; i <= segmentCount; ++i) // <= per chiudere il cerchio
    {
        float theta = static_cast<float>(i) / static_cast<float>(segmentCount) * 2.0f * 3.14159265f;
        circleVertices.push_back(centerX + radius * cosf(theta));
        circleVertices.push_back(centerY + radius * sinf(theta));
        circleVertices.push_back(0.0f);
    }

    createGeometry("circle", circleVertices, 3);

    std::vector<float> ominoData;
    const int steps = 24;

    auto appendVertex = [&ominoData](const glm::vec3 &p, const glm::vec3 &c)
    {
        ominoData.push_back(p.x);
        ominoData.push_back(p.y);
        ominoData.push_back(p.z);
        ominoData.push_back(c.r);
        ominoData.push_back(c.g);
        ominoData.push_back(c.b);
    };

    auto appendTriangle = [&appendVertex](const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c,
                                          const glm::vec3 &ca, const glm::vec3 &cb, const glm::vec3 &cc)
    {
        appendVertex(a, ca);
        appendVertex(b, cb);
        appendVertex(c, cc);
    };

    auto sampleHermiteEllipse = [this, steps](float cx, float cy, float rx, float ry)
    {
        std::vector<glm::vec3> points;
        const float m = 1.65685f;
        const float mx = m * rx;
        const float my = m * ry;

        auto hermiteSegment = [this, steps](const glm::vec3 &p0, const glm::vec3 &p1,
                                            const glm::vec3 &t0, const glm::vec3 &t1,
                                            std::vector<glm::vec3> &out)
        {
            for (int i = 0; i < steps; ++i)
            {
                float u = static_cast<float>(i) / static_cast<float>(steps);
                float h1 = 2.0f * u * u * u - 3.0f * u * u + 1.0f;
                float h2 = -2.0f * u * u * u + 3.0f * u * u;
                float h3 = u * u * u - 2.0f * u * u + u;
                float h4 = u * u * u - u * u;
                glm::vec3 p = h1 * p0 + h2 * p1 + h3 * t0 + h4 * t1;
                out.push_back(p);
            }
        };

        hermiteSegment({cx, cy + ry, 0.0f}, {cx + rx, cy, 0.0f}, {mx, 0.0f, 0.0f}, {0.0f, -my, 0.0f}, points);
        hermiteSegment({cx + rx, cy, 0.0f}, {cx, cy - ry, 0.0f}, {0.0f, -my, 0.0f}, {-mx, 0.0f, 0.0f}, points);
        hermiteSegment({cx, cy - ry, 0.0f}, {cx - rx, cy, 0.0f}, {-mx, 0.0f, 0.0f}, {0.0f, my, 0.0f}, points);
        hermiteSegment({cx - rx, cy, 0.0f}, {cx, cy + ry, 0.0f}, {0.0f, my, 0.0f}, {mx, 0.0f, 0.0f}, points);

        return points;
    };

    const glm::vec3 colorNeonCyan(0.0f, 1.0f, 1.0f);
    const glm::vec3 colorNeonMagenta(1.0f, 0.0f, 1.0f);
    const glm::vec3 colorNeonYellow(1.0f, 1.0f, 0.0f);

    auto buildHollowLoop = [&appendTriangle](const std::vector<glm::vec3> &outer,
                                             const std::vector<glm::vec3> &inner,
                                             const glm::vec3 &colorOuter,
                                             const glm::vec3 &colorInner)
    {
        int loopSize = static_cast<int>(outer.size());
        for (int i = 0; i < loopSize; ++i)
        {
            int next = (i + 1) % loopSize;
            appendTriangle(outer[i], outer[next], inner[i], colorOuter, colorOuter, colorInner);
            appendTriangle(outer[next], inner[next], inner[i], colorOuter, colorInner, colorInner);
        }
    };

    // Palla inferiore (corpo)
    std::vector<glm::vec3> ball1Outer = sampleHermiteEllipse(0.0f, -0.20f, 0.7f, 0.5f);
    std::vector<glm::vec3> ball1Inner = sampleHermiteEllipse(0.0f, -0.20f, 0.6f, 0.4f);
    buildHollowLoop(ball1Outer, ball1Inner, colorNeonCyan, colorNeonMagenta);

    // Palla centrale
    std::vector<glm::vec3> ball2Outer = sampleHermiteEllipse(0.0f, 0.0f, 0.15f, 0.15f);
    std::vector<glm::vec3> ball2Inner = sampleHermiteEllipse(0.0f, 0.0f, 0.08f, 0.08f);
    buildHollowLoop(ball2Outer, ball2Inner, colorNeonMagenta, colorNeonYellow);

    // Palla superiore (testa)
    std::vector<glm::vec3> ball3Outer = sampleHermiteEllipse(0.0f, 0.60f, 0.4f, 0.3f);
    std::vector<glm::vec3> ball3Inner = sampleHermiteEllipse(0.0f, 0.60f, 0.3f, 0.2f);
    buildHollowLoop(ball3Outer, ball3Inner, colorNeonYellow, colorNeonCyan);

    std::vector<glm::vec3> left_eyeOuter = sampleHermiteEllipse(-0.15f, 0.6f, 0.05f, 0.08f);
    std::vector<glm::vec3> left_eyeInner = sampleHermiteEllipse(-0.15f, 0.6f, 0.04f, 0.07f);
    buildHollowLoop(left_eyeOuter, left_eyeInner, colorNeonCyan, colorNeonCyan);

    std::vector<glm::vec3> right_eyeOuter = sampleHermiteEllipse(0.15f, 0.6f, 0.05f, 0.08f);
    std::vector<glm::vec3> right_eyeInner = sampleHermiteEllipse(0.15f, 0.6f, 0.04f, 0.07f);
    buildHollowLoop(right_eyeOuter, right_eyeInner, colorNeonCyan, colorNeonCyan);

    playerVertexCount = static_cast<int>(ominoData.size() / 6);
    createGeometry("player", ominoData, 6);
}

void View::createGeometry(const std::string &name, const std::vector<float> &vertices, int componentsPerVertex)
{
    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    // Attribute 0: Position (Sempre presente, primi 3 float)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, componentsPerVertex * sizeof(float), (void *)0);

    // Attribute 1: Color (Se presente, ovvero se componentsPerVertex == 6)
    if (componentsPerVertex == 6)
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    vaos[name] = vao;
    vbos[name] = vbo;
}