#version 330 core

// Fragment Shader
in vec2 vTexCoord; // Coordinate da 0.0 a 1.0 (passate dal Vertex Shader)
out vec4 FragColor;

uniform vec3 uColor;
uniform bool uIsBlock;
uniform float uBorderThickness; // es. 0.05

void main() {

    if (uIsBlock) {
        // Controlla se siamo vicino ai bordi del quadrato
        bool isBorder = vTexCoord.x < uBorderThickness || vTexCoord.x > (1.0 - uBorderThickness) ||
                            vTexCoord.y < uBorderThickness || vTexCoord.y > (1.0 - uBorderThickness);
        if (isBorder) {
            FragColor = vec4(uColor, 1.0); // Colore del bordo
        } else {
            FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Interno nero
        }
    } else {
    // Se non è un blocco (es. giocatore), coloralo tutto
        FragColor = vec4(uColor, 1.0);
    }
}