#version 330 core
in vec3 vColor;
out vec4 FragColor;
uniform bool uUseSolidColor;
uniform vec3 uSolidColor;

void main() {
    vec3 finalColor = uUseSolidColor ? uSolidColor : vColor;
    float alpha = uUseSolidColor ? 0.40 : 0.90;
    FragColor = vec4(finalColor, alpha);
}
