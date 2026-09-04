#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 uProjection; 
uniform vec2 uModel;      // Posizione nel mondo
uniform vec2 uScale;       // Dimensioni nel mondo
uniform float rows;

out vec2 TexCoords;

void main()
{
    // 1. Applichiamo trasformazioni locali (Scale + Offset)
    // Se aPos va da -0.5 a 0.5, moltiplicando per uScale e sommando uOffset 
    // otteniamo le coordinate "World Space" dell'oggetto.
    vec2 worldPos = aPos.xy * uScale + uModel;
    if (worldPos.y > rows) {
        worldPos.y = rows; // Limitiamo la posizione massima in y
    }

    // 2. La proiezione trasforma i valori del mondo in Clip Space
    gl_Position = uProjection * vec4(worldPos, 0.0, 1.0);

    // 3. TexCoords (0.0 a 1.0) per il glow nel fragment
    TexCoords = aPos.xy + 0.5; 
}