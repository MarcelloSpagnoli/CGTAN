#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float uTimeSinceHit; // Millisecondi passati dall'ultimo colpo
uniform vec3 uColor;
uniform float uMaxLife;      // Corrisponde a LASER_TIME_MILLIS (es. 150.0)


void main() {

    // 1. Calcoliamo l'intensità del flash (decadimento lineare o esponenziale)
    // Va da 1.0 (appena colpito) a 0.0 (fine vita)
    float lifeRatio = clamp(uTimeSinceHit / uMaxLife, 0.0, 1.0);
    float flashIntensity = pow(1.0 - lifeRatio, 2.0); // Quadratico per un fade-out più secco

    // 2. Disegno del raggio
    float distFromCenter = distance(TexCoords, vec2(0.5)); // Distanza dal centro del laser (0.5, 0.5)
    
    // Effetto "Core": bianco al centro, colore ai bordi
    vec3 coreColor = uColor * vec3(2.0);
    vec3 finalColor = mix(coreColor, uColor, distFromCenter);
    
    // 3. Alpha: sfumiamo i bordi e applichiamo il flash
    float edgeFade = smoothstep(1.0, 0.2, distFromCenter);
    float alpha = edgeFade * flashIntensity;
    
    FragColor = vec4(finalColor, alpha);
}