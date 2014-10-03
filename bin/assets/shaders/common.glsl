#version 430

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;
in vec3 VertexTangent;
in vec3 VertexBitangent;

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
layout(binding = 4) uniform sampler2D OpacityMap;

uniform bool DiffuseMapIsUsed;
uniform bool NormalMapIsUsed;
uniform bool HeightMapIsUsed;
uniform bool SpecularMapIsUsed;
uniform bool OpacityMapIsUsed;

struct LightInfo
{
	vec4 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};
uniform LightInfo Lights[8];

struct MaterialInfo
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};
MaterialInfo Material;

/*
Material.ambient = vec4(0.3, 0.3, 0.3, 1.0);
Material.diffuse = vec4(0.7, 0.7, 0.7, 1.0);
Material.specular = vec4(0.5, 0.5, 0.5, 1.0);
Material.shininess = 60.0;
*/
