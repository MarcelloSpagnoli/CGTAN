#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>

enum class ShaderType
{
    Vertex,
    Fragment
};

class ShaderProgram
{
public:
    ShaderProgram() = default;
    ~ShaderProgram();

    // Eliminiamo costruttore di copia e operatore di assegnazione (Rule of Three/Five)
    // Un programma shader è una risorsa GPU univoca e non deve essere duplicata accidentalmente.
    ShaderProgram(const ShaderProgram &) = delete;
    ShaderProgram &operator=(const ShaderProgram &) = delete;

    // Supporto per il trasferimento (Move semantics)
    ShaderProgram(ShaderProgram &&other) noexcept;
    ShaderProgram &operator=(ShaderProgram &&other) noexcept;

    // Metodi principali
    void loadFromSource(const std::string &vertexSource, const std::string &fragmentSource);
    void use() const;

    // Utility
    int uniformLocation(const char *name) const;
    unsigned int id() const;
    bool isValid() const;
    void reset();
    void loadFromFiles(const std::string &vertexPath, const std::string &fragmentPath);

private:
    unsigned int programId = 0;
    unsigned int compileShader(ShaderType type, const std::string &source);
};

#endif // SHADER_PROGRAM_H
