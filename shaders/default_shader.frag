#version 410 core

layout(std140) uniform Material {
    vec4 materialColor;
    int useTexture;
};

uniform sampler2D textureAtlas;
uniform sampler2D lightmapAtlas;

in vec4 vertexColor;
in vec2 lmCoord;
out vec4 fragColor;

void main() {
    vec4 baseColor = vec4(1, 1, 1, 1);
    vec4 lightColor = texture(lightmapAtlas, lmCoord);

    fragColor = baseColor * lightColor;
}