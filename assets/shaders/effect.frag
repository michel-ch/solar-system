#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D u_texture;
uniform vec2 u_textureSize;
uniform int effectType;

// Effet 1: Simple (pas d'effet)
vec4 simpleEffect() {
    return texture(u_texture, TexCoords);
}

// Effet 2: Flou gaussien
vec4 blurEffect() {
    vec2 texelSize = 1.0 / u_textureSize;
    vec4 color = vec4(0.0);
    
    // Matrice de flou 3x3
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            color += texture(u_texture, TexCoords + offset);
        }
    }
    
    return color / 9.0;
}

// Effet 3: Edge detection (Sobel)
vec4 edgeDetection() {
    vec2 texelSize = 1.0 / u_textureSize;
    
    float sobelX[9] = float[](
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    );
    
    float sobelY[9] = float[](
        -1, -2, -1,
         0,  0,  0,
         1,  2,  1
    );
    
    float edgeX = 0.0;
    float edgeY = 0.0;
    
    for(int i = 0; i < 9; i++) {
        int x = i % 3 - 1;
        int y = i / 3 - 1;
        vec2 offset = vec2(float(x), float(y)) * texelSize;
        vec3 color = texture(u_texture, TexCoords + offset).rgb;
        float gray = dot(color, vec3(0.299, 0.587, 0.114));
        
        edgeX += gray * sobelX[i];
        edgeY += gray * sobelY[i];
    }
    
    float edge = sqrt(edgeX * edgeX + edgeY * edgeY);
    return vec4(vec3(edge), 1.0);
}

// Effet 4: Inversion des couleurs
vec4 invertEffect() {
    vec4 color = texture(u_texture, TexCoords);
    return vec4(1.0 - color.rgb, color.a);
}

// Effet 5: Grayscale
vec4 grayscaleEffect() {
    vec4 color = texture(u_texture, TexCoords);
    float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    return vec4(vec3(gray), color.a);
}

// Effet 6: Contraste élevé
vec4 highContrastEffect() {
    vec4 color = texture(u_texture, TexCoords);
    color.rgb = (color.rgb - 0.5) * 2.0 + 0.5;
    return color;
}

// Effet 7: Vignette
vec4 vignetteEffect() {
    vec4 color = texture(u_texture, TexCoords);
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(TexCoords, center);
    float vignette = 1.0 - smoothstep(0.3, 0.8, dist);
    return color * vignette;
}

void main()
{
    if (effectType == 0) {
        FragColor = simpleEffect();
    } else if (effectType == 1) {
        FragColor = blurEffect();
    } else if (effectType == 2) {
        FragColor = edgeDetection();
    } else if (effectType == 3) {
        FragColor = invertEffect();
    } else if (effectType == 4) {
        FragColor = grayscaleEffect();
    } else if (effectType == 5) {
        FragColor = highContrastEffect();
    } else if (effectType == 6) {
        FragColor = vignetteEffect();
    } else {
        FragColor = simpleEffect();
    }
}