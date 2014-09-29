#version 430

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;
in vec4 VertexTangent;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 MVP;
uniform mat3 NormalMatrix;

layout(binding = 0) uniform sampler2D TexDiffuse;
layout(binding = 1) uniform sampler2D TexNormal;
layout(binding = 2) uniform sampler2D TexSpecular;

/*uniform*/ struct LightInfo
{
	vec3 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
} Light;

/*uniform*/ struct MaterialInfo
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
} Material;