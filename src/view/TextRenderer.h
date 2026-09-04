#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <vector>
#include "shaders/Shader.h"

struct Character
{
    unsigned int textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};

struct TextVertex
{
    float x, y;    // position
    float u, v;    // texture coords
    float r, g, b; // color
    float texID;   // texture ID
};

class TextRenderer
{
public:
    TextRenderer(const std::string &fontPath, int fontSize);
    ~TextRenderer();
    void beginBatch();
    void addText(const std::string &text, float x, float y, glm::vec3 color);
    void renderBatch();
    void setProjection(const glm::mat4 &projection) { this->projection = projection; }

private:
    FT_Library ft;
    FT_Face face;
    std::map<char, Character> characters;
    unsigned int VAO, VBO;
    ShaderProgram textShader;
    glm::mat4 projection;

    // Cache uniform locations
    int locText, locProjection, locTextColor;

    // Batch data
    std::vector<TextVertex> vertices;
    int vertexCount = 0;
};

#endif