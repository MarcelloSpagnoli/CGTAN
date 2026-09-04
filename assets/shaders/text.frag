#version 330 core
in vec2 vTexCoord;
in vec3 vColor;
out vec4 color;

uniform sampler2D text;

void main()
{    
    float sampled = texture(text, vTexCoord).r;
    color = vec4(vColor, sampled);
}