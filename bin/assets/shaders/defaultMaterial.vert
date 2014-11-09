#include "common.glsl"

//out vec3 LightDir;
//out vec3 ViewDir;
out vec2 TexCoord;

smooth out vec4 VIEW_POSITION;
noperspective out vec3 VIEW_NORMAL;

void main()
{
	VIEW_NORMAL = NormalMatrix * VertexNormal;
	VIEW_POSITION = ModelViewMatrix * vec4(VertexPosition, 1.0);
	TexCoord = VertexTexCoord * TilingUV;
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}
