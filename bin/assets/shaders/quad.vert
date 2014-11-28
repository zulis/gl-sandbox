#include "common.glsl"

out vec2 TexCoord;

void main()
{
	gl_Position = MVP * vec4(VertexPosition.x, VertexPosition.y, 0.0, 1.0);
	TexCoord = VertexTexCoord;
}
