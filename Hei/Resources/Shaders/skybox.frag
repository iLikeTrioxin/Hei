#version 450 core

struct Material {
    sampler2D diffuseMap ;
    sampler2D specularMap;
    float     shininess  ;
};

struct Light {
// None - 0 || dirLight - 1 || pointLight - 2 || spotLight - 3
    vec3  position;  // For spotLight and pointLight
    vec3  direction; // For dirLight and spotLight
    float lightType; // For everyone (treat as int - uniform buffer cannot contain int)

    float cutOff;      // For spotLight
    float outerCutOff; // For spotLight

    float constant;    // For spotLight and pointLight
    float linear;      // For spotLight and pointLight
    float quadratic;   // For spotLight and pointLight

    vec3 ambient;      // For everyone
    vec3 diffuse;      // For everyone
    vec3 specular;     // For everyone
};

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec3 _FragPos;
layout (location = 1) in vec3 _Normal;
layout (location = 2) in vec3 _TexCoords;

layout (binding = 0) uniform           u_shininess { float shininess; };
layout (binding = 1) uniform sampler2D diffuseMap ;
layout (binding = 2) uniform sampler2D specularMap;

layout (binding = 3) uniform samplerCube texture_diffuse1 ;
layout (binding = 4) uniform sampler2D texture_diffuse2 ;
layout (binding = 5) uniform sampler2D texture_specular1;
layout (binding = 6) uniform sampler2D texture_normal1  ;
layout (binding = 7) uniform sampler2D texture_height1  ;

layout (binding = 8) uniform u_viewPos           { vec3     viewPos;  };
layout (binding = 9) uniform u_light             { Light    light[4]; };

void main() {
    vec4 color = texture(texture_diffuse1, _FragPos);

    //if(color.w < 0.05) discard;

    vec3 normal      = normalize(_Normal);
    vec3 viewDir     = normalize(viewPos - _FragPos);

    //vec3 lightResult = CalcDirLight(light[0], normal, viewDir);

    FragColor = color;//vec4(1.0f);//color * vec4(lightResult, 1.0f);
}