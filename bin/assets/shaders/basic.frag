#include "common.glsl"

in vec2 TexCoord;
in vec3 Position;
in vec3 Normal;
out vec4 FragColor;

void main()
{
	if(ColorMapIsUsed)
		FragColor = texture(ColorMap, TexCoord);
	else
		FragColor = vec4(Normal, 1);
}