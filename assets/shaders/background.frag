#version 330 core

out vec4 FragColor;

uniform vec2 uMouse;
uniform float uTime;
uniform vec2 uResolution;
uniform vec2 uViewportOffset;
uniform float rows;

void main() {
    vec2 uv = (gl_FragCoord.xy - uViewportOffset) / uResolution.xy;
    float aspect = uResolution.x / uResolution.y;

    float borderY = 0.001; 
    float borderX = borderY / aspect; 
    float rowHeight = 1.0 / rows;
    float contentTop = 1.0 - rowHeight; // Il limite dove finiscono i punti e inizia la riga nera

    // 1. RIGA NERA (In alto)
    if (uv.y >= contentTop) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // 2. CONTROLLO BORDI (Solo nell'area sotto la riga nera)
    // Sinistra, Destra, Basso e il "Soffitto" sotto la riga nera
    if (uv.x <= borderX || uv.x >= (1.0 - borderX) || 
        uv.y <= borderY || uv.y >= (contentTop - borderY)) 
    {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        return;
    }

    // 3. AREA PUNTINI
    vec2 uv_corr = vec2(uv.x * aspect, uv.y);
    vec2 mouse_corr = vec2(uMouse.x * aspect, uMouse.y);

    float mouseDist = distance(uv_corr, mouse_corr);
    vec2 grid_uv = fract(uv_corr * rows);
    float dist = distance(grid_uv, vec2(0.5));
    
    float pulse = 0.02 + 0.01 * sin(uTime + uv.x * 10.0);
    
    float interactionRadius = 0.2; 
    if (mouseDist < interactionRadius) {
        float strength = 1.0 - (mouseDist / interactionRadius);
        pulse += 0.06 * strength;
    }
    
    float dots = smoothstep(pulse, pulse - 0.01, dist);
    vec3 color = vec3(dots * 0.6);
    FragColor = vec4(color, 1.0);
}