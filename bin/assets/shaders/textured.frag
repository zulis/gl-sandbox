#include "common.glsl"

//in vec3 Color;
in vec2 TexCoord;

layout( location = 0 ) out vec4 FragColor;

void main() {
    FragColor = texture(DiffuseMap, TexCoord);
}