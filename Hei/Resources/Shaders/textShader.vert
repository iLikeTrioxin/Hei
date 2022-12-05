#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCords;
layout (location = 2) in int  texIndex;

layout (location = 0) out vec2 TexCoords;
layout (location = 1) out int TexIndex;

layout (binding = 0) uniform View {
    mat4 model;
    mat4 projection;
    mat4 view;
} v;

void main() {

    gl_Position = v.projection * v.view * vec4(vec3(v.model * vec4(position, 1.0)), 1.0);

    //gl_Position = v.projection * vec4(position, 1.0f);
    TexCoords = texCords;
    TexIndex = texIndex;
}