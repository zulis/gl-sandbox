#include "common.glsl"

out vec2 TexCoord;
out vec3 ViewPosition;
out vec3 ViewNormal;
out mat3 TBN;

void main()
{
	vec4 position = vec4(VertexPosition, 1.0);
	ViewNormal = NormalMatrix * VertexNormal;
	ViewPosition = vec3(ModelViewMatrix * position);
	TexCoord = VertexTexCoord * TilingUV;
	gl_Position = MVP * position;
	
	if(NormalMapIsUsed)
	{
		vec3 n = normalize(NormalMatrix * VertexNormal);
		vec3 t = normalize(NormalMatrix * vec3(VertexTangent));
		vec3 b = normalize(cross(n, t)) * VertexTangent.w;
	
		TBN = mat3(
			t.x, b.x, n.x,
			t.y, b.y, n.y,
			t.z, b.z, n.z );
	}
}
