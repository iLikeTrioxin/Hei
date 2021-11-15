#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in int aTexIndex;

uniform mat4 projection;
out vec2 texCoords;
flat out int texIndex;

void main() {
    texIndex = aTexIndex;
    texCoords = aTexCoords;
    gl_Position = projection * vec4(aPos, 1.0f, 1.0f);
}
