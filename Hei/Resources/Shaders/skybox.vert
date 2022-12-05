#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

layout (location = 0) out vec3 _FragPos;
layout (location = 1) out vec3 _Normal;
layout (location = 2) out vec3 _TexCoords;

layout (binding = 0) uniform View{
    mat4 model;
    mat4 projection;
    mat4 view;
} v;

void main() {
    mat4 view = mat4(mat3(v.view));

    _FragPos = vec3(v.model * vec4(aPos, 1.0));
    _Normal  = mat3(v.model) * aNormal;
    _TexCoords = aPos;
    gl_Position = (v.projection * view * vec4(aPos, 1.0)).xyww;
}