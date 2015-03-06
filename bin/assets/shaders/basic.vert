#include "common.glsl"

out vec2 TexCoord;
out vec3 Position;
out vec3 Normal;

void main()
{
	TexCoord = VertexTexCoord;
	vec4 position = ModelViewMatrix * vec4(VertexPosition, 1.0);
	Position = position.xyz;
	Normal = normalize(NormalMatrix * VertexNormal);
	gl_Position = ProjectionMatrix * position;
	
}