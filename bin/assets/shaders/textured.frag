#include "common.glsl"

//in vec3 Color;
in vec2 TexCoord;

layout( location = 0 ) out vec4 FragColor;

void main() {
	if(OpacityMapIsUsed && texture(OpacityMap, TexCoord).r == 0.0)
		discard;
		
	vec4 finalColor = texture(DiffuseMap, TexCoord);
	
	FragColor = finalColor;
}