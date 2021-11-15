#version 330 core

out vec4 fragColor;

in vec2 texCoords;
flat in int texIndex;

uniform sampler2D textures[32];

void main() {
    fragColor = vec4(texture(textures[texIndex], texCoords).r);
}