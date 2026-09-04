#include "Shader.h"
#include <stdexcept>
#include <string>
#include <glad/glad.h>
#include <fstream>
#include <sstream>

unsigned int ShaderProgram::compileShader(ShaderType type, const std::string &source)
{
    const unsigned int shaderType = (type == ShaderType::Vertex) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
    const unsigned int shaderId = glCreateShader(shaderType);
    const char *sourceCStr = source.c_str();

    // Carica il codice sorgente(stringa C) nell'oggetto shader.
    //  Parametri: ID shader, numero di stringhe (1), indirizzo della stringa, lunghezza (NULL = terminata da zero)
    glShaderSource(shaderId, 1, &sourceCStr, nullptr);

    glCompileShader(shaderId); // Compila il codice sorgente appena caricato

    int ok = false;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &ok);
    if (ok)
    {
        return shaderId;
    }
    int logLen = 0;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLen);
    std::string log(logLen > 0 ? logLen : 1, '\0');
    glGetShaderInfoLog(shaderId, logLen, nullptr, &log[0]);

    glDeleteShader(shaderId);
    throw std::runtime_error("Shader compile error: " + log);
}

void ShaderProgram::loadFromSource(const std::string &vertexSource, const std::string &fragmentSource)
{
    reset();

    const unsigned int vertexId = compileShader(ShaderType::Vertex, vertexSource);
    const unsigned int fragmentId = compileShader(ShaderType::Fragment, fragmentSource);

    this->programId = glCreateProgram();
    glAttachShader(this->programId, vertexId);
    glAttachShader(this->programId, fragmentId);
    glLinkProgram(this->programId);

    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);

    int ok = 0;
    glGetProgramiv(programId, GL_LINK_STATUS, &ok);
    if (ok == GL_TRUE)
    {
        return;
    }

    int logLen = 0;
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLen);

    std::string log(logLen > 0 ? logLen : 1, '\0');
    glGetProgramInfoLog(programId, logLen, nullptr, &log[0]);

    glDeleteProgram(programId);
    programId = 0;

    throw std::runtime_error("Shader link error: " + log);
}

ShaderProgram::~ShaderProgram()
{
    reset();
}

ShaderProgram::ShaderProgram(ShaderProgram &&other) noexcept
    : programId(other.programId)
{
    other.programId = 0;
}

ShaderProgram &ShaderProgram::operator=(ShaderProgram &&other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    reset();
    programId = other.programId;
    other.programId = 0;
    return *this;
}

void ShaderProgram::loadFromFiles(const std::string &vertexPath, const std::string &fragmentPath)
{
    std::ifstream vFile(vertexPath);
    if (!vFile.is_open())
    {
        throw std::runtime_error("Cannot open shader file: " + vertexPath);
    }

    std::ifstream fFile(fragmentPath);
    if (!fFile.is_open())
    {
        throw std::runtime_error("Cannot open shader file: " + fragmentPath);
    }

    std::ostringstream vStream;
    vStream << vFile.rdbuf();

    std::ostringstream fStream;
    fStream << fFile.rdbuf();

    loadFromSource(vStream.str(), fStream.str());
}

void ShaderProgram::use() const
{
    glUseProgram(programId);
}

int ShaderProgram::uniformLocation(const char *name) const
{
    return glGetUniformLocation(programId, name);
}

unsigned int ShaderProgram::id() const
{
    return programId;
}

bool ShaderProgram::isValid() const
{
    return programId != 0;
}

void ShaderProgram::reset()
{
    if (programId != 0)
    {
        glDeleteProgram(programId);
        programId = 0;
    }
}
