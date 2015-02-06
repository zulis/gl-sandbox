#include "common.glsl"

out vec2 TexCoord;

void main()
{
	TexCoord = VertexTexCoord;
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}