#include "common.glsl"

out vec4 vVertex;
out vec3 vNormal;
out vec2 vTexCoord;
out mat3 mTBN;
out mat4 mViewMatrix;

//out vec3 ViewPosition;
void main()
{
	// Calculate view space position (required for lighting)
	vVertex = ModelViewMatrix * vec4(VertexPosition, 1.0);
	// Calculate view space normal (required for lighting & normal mapping)
	vNormal = normalize(NormalMatrix * VertexNormal);
	// Pass texture coordinates
	vTexCoord = VertexTexCoord * TilingUV;
	
	mViewMatrix = ViewMatrix;
	
	if(NormalMapIsUsed)
	{
		vec3 n = normalize(NormalMatrix * VertexNormal);
		vec3 t = normalize(NormalMatrix * vec3(VertexTangent));
		vec3 b = normalize(cross(n, t)) * VertexTangent.w;
	
		mTBN = mat3(
			t.x, b.x, n.x,
			t.y, b.y, n.y,
			t.z, b.z, n.z );
	}
	
	// Vertex shader must always pass projection space position
	gl_Position = MVP * vec4(VertexPosition, 1.0);
}
