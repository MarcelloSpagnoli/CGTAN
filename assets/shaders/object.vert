#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec2 vTexCoord;

uniform mat4 uProjection;
uniform mat4 uModel;
uniform mat4 uScale;

void main() {
    vTexCoord = aPos.xy + vec2(0.5); // Attenzione: questo ha senso solo su quad centrati
    gl_Position = uProjection * uModel * uScale * vec4(aPos, 1.0);
}