#include "common.glsl"

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texDiffuse;

void main()
{
	FragColor = texture2D(texDiffuse, TexCoord);
}
