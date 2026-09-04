#include "TextRenderer.h"
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

TextRenderer::TextRenderer(const std::string &fontPath, int fontSize)
{
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (GLuint)face->glyph->advance.x};
        characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TextVertex) * 6000, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(2 * sizeof(float)));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(4 * sizeof(float)));
    
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)(7 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    textShader.loadFromFiles(ASSETS_DIR "/shaders/text.vert", ASSETS_DIR "/shaders/text.frag");
    locText = textShader.uniformLocation("text");
    locProjection = textShader.uniformLocation("uProjection");
    locTextColor = textShader.uniformLocation("textColor");
}

TextRenderer::~TextRenderer()
{
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void TextRenderer::beginBatch()
{
    vertices.clear();
    vertexCount = 0;
}

void TextRenderer::addText(const std::string &text, float x, float y, glm::vec3 color)
{
    float scale = 0.01f;
    float totalWidth = 0.0f;
    
    for (char c : text)
    {
        Character ch = characters[c];
        totalWidth += (ch.advance >> 6) * scale;
    }
    
    float currentX = x - (totalWidth / 2.0f);
    
    for (char c : text)
    {
        Character ch = characters[c];
        
        float xpos = currentX + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        float texID = (float)ch.textureID;
        
        // 6 vertices per quad
        TextVertex v[6] = {
            {xpos, ypos + h, 0.0f, 0.0f, color.x, color.y, color.z, texID},
            {xpos, ypos, 0.0f, 1.0f, color.x, color.y, color.z, texID},
            {xpos + w, ypos, 1.0f, 1.0f, color.x, color.y, color.z, texID},
            {xpos, ypos + h, 0.0f, 0.0f, color.x, color.y, color.z, texID},
            {xpos + w, ypos, 1.0f, 1.0f, color.x, color.y, color.z, texID},
            {xpos + w, ypos + h, 1.0f, 0.0f, color.x, color.y, color.z, texID},
        };
        
        for (int i = 0; i < 6; i++)
        {
            vertices.push_back(v[i]);
        }
        vertexCount += 6;
        
        currentX += (ch.advance >> 6) * scale;
    }
}

void TextRenderer::renderBatch()
{
    if (vertexCount == 0) return;
    
    textShader.use();
    glUniformMatrix4fv(locProjection, 1, GL_FALSE, glm::value_ptr(projection));
    
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(TextVertex), vertices.data());
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Raggruppa i vertex per texture e drawcall
    if (!vertices.empty())
    {
        GLuint currentTex = (GLuint)vertices[0].texID;
        int startIndex = 0;
        
        for (int i = 0; i <= vertices.size(); i++)
        {
            GLuint nextTex = (i < vertices.size()) ? (GLuint)vertices[i].texID : 0;
            
            if (nextTex != currentTex || i == vertices.size())
            {
                glBindTexture(GL_TEXTURE_2D, currentTex);
                glDrawArrays(GL_TRIANGLES, startIndex, i - startIndex);
                startIndex = i;
                currentTex = nextTex;
            }
        }
    }
    
    glDisable(GL_BLEND);
    glBindVertexArray(0);
}
