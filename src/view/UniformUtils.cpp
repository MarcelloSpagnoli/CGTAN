#include "UniformUtils.h"

#include <type_traits>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace view
{
    RenderCommand::UniformValue makeUniformValue(float value)
    {
        return RenderCommand::UniformValue(std::in_place_type<float>, value);
    }

    RenderCommand::UniformValue makeUniformValue(int value)
    {
        return RenderCommand::UniformValue(std::in_place_type<int>, value);
    }

    RenderCommand::UniformValue makeUniformValue(unsigned int value)
    {
        return RenderCommand::UniformValue(std::in_place_type<unsigned int>, value);
    }

    RenderCommand::UniformValue makeUniformValue(const glm::vec2 &value)
    {
        return RenderCommand::UniformValue(std::in_place_type<glm::vec2>, value);
    }

    RenderCommand::UniformValue makeUniformValue(const glm::vec3 &value)
    {
        return RenderCommand::UniformValue(std::in_place_type<glm::vec3>, value);
    }

    RenderCommand::UniformValue makeUniformValue(const glm::vec4 &value)
    {
        return RenderCommand::UniformValue(std::in_place_type<glm::vec4>, value);
    }

    RenderCommand::UniformValue makeUniformValue(const glm::mat3 &value)
    {
        return RenderCommand::UniformValue(std::in_place_type<glm::mat3>, value);
    }

    RenderCommand::UniformValue makeUniformValue(const glm::mat4 &value)
    {
        return RenderCommand::UniformValue(std::in_place_type<glm::mat4>, value);
    }

    void applyUniform(const ShaderProgram &shader, const std::string &name, const RenderCommand::UniformValue &value)
    {
        const int location = shader.uniformLocation(name.c_str());
        if (location < 0)
        {
            return;
        }

        std::visit(
            [location](const auto &uniformValue)
            {
                using T = std::decay_t<decltype(uniformValue)>;
                if constexpr (std::is_same_v<T, float>)
                {
                    glUniform1f(location, uniformValue);
                }
                else if constexpr (std::is_same_v<T, int>)
                {
                    glUniform1i(location, uniformValue);
                }
                else if constexpr (std::is_same_v<T, unsigned int>)
                {
                    glUniform1ui(location, uniformValue);
                }
                else if constexpr (std::is_same_v<T, glm::vec2>)
                {
                    glUniform2f(location, uniformValue.x, uniformValue.y);
                }
                else if constexpr (std::is_same_v<T, glm::vec3>)
                {
                    glUniform3f(location, uniformValue.x, uniformValue.y, uniformValue.z);
                }
                else if constexpr (std::is_same_v<T, glm::vec4>)
                {
                    glUniform4f(location, uniformValue.x, uniformValue.y, uniformValue.z, uniformValue.w);
                }
                else if constexpr (std::is_same_v<T, glm::mat3>)
                {
                    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(uniformValue));
                }
                else if constexpr (std::is_same_v<T, glm::mat4>)
                {
                    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(uniformValue));
                }
            },
            value);
    }
} // namespace view
