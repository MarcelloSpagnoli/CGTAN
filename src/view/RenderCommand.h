#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <variant>
#include <utility>

struct RenderCommand
{
    // Tipi uniform supportati (bastano per quasi tutti i casi base).
    using UniformValue = std::variant<float, int, unsigned int, bool, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4>;
    using UniformMap = std::unordered_map<std::string, UniformValue>;

    // Nome shader da usare (chiave nella mappa shaderPrograms).
    std::string shaderName;

    // Nome geometria da usare (chiave nella mappa vaos/vbos).
    std::string geometryName;

    // Uniform specifiche di questo oggetto (es: colore, trasformazione, hp, ecc.).
    UniformMap uniforms;

    // Numero vertici da passare a glDrawArrays.
    int vertexCount = 0;

    // Primitiva OpenGL (es: GL_TRIANGLES).
    unsigned int primitive = 0;

    // Spessore linea per primitive lineari (GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP).
    float lineWidth = 1.0f;

    // Helper inline per creare UniformValue
    template <typename T>
    static inline UniformValue makeUniformValue(const T &value) 
    { 
        return UniformValue(std::in_place_type<T>, value); 
    }
};

#endif
