#include "common.glsl"

in vec2 TexCoord;
out vec4 FragColor;

void main()
{
	if(ColorMapIsUsed)
		FragColor = texture(ColorMap, TexCoord);
	else
		FragColor = vec4(1, 1, 1, 1);
}