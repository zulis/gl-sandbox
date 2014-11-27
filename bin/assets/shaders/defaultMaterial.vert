#include "common.glsl"

//out vec3 LightDir;
//out vec3 ViewDir;
out vec2 TexCoord;

smooth out vec3 VIEW_POSITION;
noperspective out vec3 VIEW_NORMAL;
out mat3 TBN;

void main()
{
	VIEW_NORMAL = NormalMatrix * VertexNormal;
	VIEW_POSITION = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
	TexCoord = VertexTexCoord * TilingUV;
	gl_Position = MVP * vec4(VertexPosition, 1.0);
	
	vec3 norm = normalize(NormalMatrix * VertexNormal);
    vec3 tang = normalize(NormalMatrix * vec3(VertexTangent));
	vec3 binormal = normalize(cross(norm, tang)) * VertexTangent.w;
	
	TBN = mat3(
        tang.x, binormal.x, norm.x,
        tang.y, binormal.y, norm.y,
        tang.z, binormal.z, norm.z ) ;
}
