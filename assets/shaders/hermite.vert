#version 330 core

layout (location = 0) in vec3 aPos;   // Dai dati Hermite (3 float)
layout (location = 1) in vec3 aColor; // Dai dati Hermite (altri 3 float)

uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uScale;

out vec3 vColor;

void main()
{
    vColor = aColor;
    
    // Usa la stessa logica degli altri shader
    // Moltiplica le matrici per la posizione del vertice
    gl_Position = uProjection * uModel * uScale * vec4(aPos, 1.0);
}