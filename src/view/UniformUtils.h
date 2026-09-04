#ifndef UNIFORM_UTILS_H
#define UNIFORM_UTILS_H

#include "RenderCommand.h"
#include "shaders/Shader.h"
#include <string>

namespace view
{
    RenderCommand::UniformValue makeUniformValue(float value);
    RenderCommand::UniformValue makeUniformValue(int value);
    RenderCommand::UniformValue makeUniformValue(unsigned int value);
    RenderCommand::UniformValue makeUniformValue(const glm::vec2 &value);
    RenderCommand::UniformValue makeUniformValue(const glm::vec3 &value);
    RenderCommand::UniformValue makeUniformValue(const glm::vec4 &value);
    RenderCommand::UniformValue makeUniformValue(const glm::mat3 &value);
    RenderCommand::UniformValue makeUniformValue(const glm::mat4 &value);

    void applyUniform(const ShaderProgram &shader, const std::string &name, const RenderCommand::UniformValue &value);
} // namespace view

#endif
