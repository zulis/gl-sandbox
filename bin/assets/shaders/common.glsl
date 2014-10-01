#version 430

in vec3 VertexPosition;
in vec3 VerNormalMap;
in vec2 VertexTexCoord;
in vec4 VertexTangent;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform mat3 NormalMatrix;

layout(binding = 0) uniform sampler2D DiffuseMap;
layout(binding = 1) uniform sampler2D NormalMap;
layout(binding = 2) uniform sampler2D HeightMap;
layout(binding = 3) uniform sampler2D SpecularMap;

uniform bool DiffuseMapIsUsed;
uniform bool NormalMapIsUsed;
uniform bool HeightMapIsUsed;
uniform bool SpecularMapIsUsed;

struct LightInfo
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};
uniform LightInfo Lights[8];

/*uniform*/ struct MaterialInfo
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
} Material;