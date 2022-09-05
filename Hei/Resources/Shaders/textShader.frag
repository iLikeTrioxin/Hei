#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;

const float width = 0.4;
const float edge = 0.1;

void main() {
    float distance = 1.0 - texture(text, TexCoords).r;
    float alpha = 1 - smoothstep(width, width + edge, distance);

    //vec4 result = vec4(1,1,1,1);
    //float mask = texture(text, TexCoords).r;
    //mask = smoothstep(0, 1, mask);
    //if( mask >= 0.7 ) { result.a = 1; }
    //else { result.a = 0; }
    color = vec4(1,1,1,1);
    //float mask = texture(text, TexCoords).a;
    //color = vec4(mask,0,0,1);
    //color = vec4(texture(text, TexCoords).r);
}
