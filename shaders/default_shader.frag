#version 450 core

layout(std140, binding = 1) uniform Material {
    vec4 materialColor;
    int useTexture;
};

uniform sampler2D textureImage;

in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

void main() {
    fragColor = vertexColor * vec4(1, 1, 1, 1);
    if(useTexture != 0) {
        fragColor = texture(textureImage, texCoord);
    }
}