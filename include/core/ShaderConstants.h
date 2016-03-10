#pragma once

#include <string>

class ShaderConstants
{
public:
	// VERTEX
	static const std::string VertexPosition;
	static const std::string VertexNormal;
	static const std::string VertexTangent;
	static const std::string VertexBitangent;
	static const std::string VertexTexCoord;
	// MATRIX
	static const std::string ProjectionMatrix;
	static const std::string ViewMatrix;
	static const std::string ModelMatrix;
	static const std::string ModelViewMatrix;
	static const std::string NormalMatrix;
	static const std::string MVP;
	// MATERIAL
	static const std::string MaterialAmbient;
	static const std::string MaterialDiffuse;
	static const std::string MaterialSpecular;
	static const std::string MaterialShininess;
	// LIGHT
	static const std::string LightPosition;
	static const std::string LightLookAt;
	static const std::string LightAmbient;
	static const std::string LightDiffuse;
	static const std::string LightSpecular;
	static const std::string LightAttenuation;
	static const std::string LightCutoff;
	static const std::string LightExponent;
	//static const std::string LightIntensity;
	static const std::string TotalLights;
	// TEXTURE
	static const std::string ColorMap;
	static const std::string NormalMap;
	static const std::string HeightMap;
	static const std::string SpecularMap;
	static const std::string EmissiveMap;
	static const std::string OpacityMap;
	static const std::string ColorMapIsUsed;
	static const std::string NormalMapIsUsed;
	static const std::string HeightMapIsUsed;
	static const std::string SpecularMapIsUsed;
	static const std::string EmissiveMapIsUsed;
	static const std::string OpacityMapIsUsed;
	// UV
	static const std::string TilingU;
	static const std::string TilingV;
	static const std::string TilingUV;
	// CORE shader source
	static const std::string CoreShaderSource;
};

// VERTEX
const std::string ShaderConstants::VertexPosition = std::string("VertexPosition");
const std::string ShaderConstants::VertexNormal = std::string("VertexNormal");
const std::string ShaderConstants::VertexTangent = std::string("VertexTangent");
const std::string ShaderConstants::VertexBitangent = std::string("VertexBitangent");
const std::string ShaderConstants::VertexTexCoord = std::string("VertexTexCoord");
// MATRIX
const std::string ShaderConstants::ProjectionMatrix = std::string("ProjectionMatrix");
const std::string ShaderConstants::ViewMatrix = std::string("ViewMatrix");
const std::string ShaderConstants::ModelMatrix = std::string("ModelMatrix");
const std::string ShaderConstants::ModelViewMatrix = std::string("ModelViewMatrix");
const std::string ShaderConstants::NormalMatrix = std::string("NormalMatrix");
const std::string ShaderConstants::MVP = std::string("MVP");
// MATERIAL
const std::string ShaderConstants::MaterialAmbient = std::string("Material.ambient");
const std::string ShaderConstants::MaterialDiffuse = std::string("Material.diffuse");
const std::string ShaderConstants::MaterialSpecular = std::string("Material.specular");
const std::string ShaderConstants::MaterialShininess = std::string("Material.shininess");
// LIGHT
const std::string ShaderConstants::LightPosition = std::string("Lights[#].position");
const std::string ShaderConstants::LightLookAt = std::string("Lights[#].lookAt");
const std::string ShaderConstants::LightAmbient = std::string("Lights[#].ambient");
const std::string ShaderConstants::LightDiffuse = std::string("Lights[#].diffuse");
const std::string ShaderConstants::LightSpecular = std::string("Lights[#].specular");
const std::string ShaderConstants::LightAttenuation = std::string("Lights[#].attenuation");
const std::string ShaderConstants::LightCutoff = std::string("Lights[#].cutoff");
const std::string ShaderConstants::LightExponent = std::string("Lights[#].exponent");
//const std::string ShaderConstants::LightIntensity = std::string("Light.Intensity");
const std::string ShaderConstants::TotalLights = std::string("TotalLights");
// TEXTURE
const std::string ShaderConstants::ColorMap = std::string("ColorMap");
const std::string ShaderConstants::NormalMap = std::string("NormalMap");
const std::string ShaderConstants::HeightMap = std::string("HeightMap");
const std::string ShaderConstants::SpecularMap = std::string("SpecularMap");
const std::string ShaderConstants::EmissiveMap = std::string("EmissiveMap");
const std::string ShaderConstants::OpacityMap = std::string("OpacityMap");
const std::string ShaderConstants::ColorMapIsUsed = std::string("ColorMapIsUsed");
const std::string ShaderConstants::NormalMapIsUsed = std::string("NormalMapIsUsed");
const std::string ShaderConstants::HeightMapIsUsed = std::string("HeightMapIsUsed");
const std::string ShaderConstants::SpecularMapIsUsed = std::string("SpecularMapIsUsed");
const std::string ShaderConstants::EmissiveMapIsUsed = std::string("EmissiveMapIsUsed");
const std::string ShaderConstants::OpacityMapIsUsed = std::string("OpacityMapIsUsed");
// UV
const std::string ShaderConstants::TilingU = std::string("TilingU");
const std::string ShaderConstants::TilingV = std::string("TilingV");
const std::string ShaderConstants::TilingUV = std::string("TilingUV");
// CORE shader source
const std::string ShaderConstants::CoreShaderSource = R"(
	#version 430
	#pragma optionNV(unroll all)

	in vec3 VertexPosition;
	in vec3 VertexNormal;
	in vec2 VertexTexCoord;
	in vec4 VertexTangent;
	in vec3 VertexBitangent;

	uniform mat4 ProjectionMatrix;
	uniform mat4 ViewMatrix;
	uniform mat4 ModelMatrix;
	uniform mat4 ModelViewMatrix;
	uniform mat4 MVP;
	uniform mat3 NormalMatrix;

	layout(binding = 0) uniform sampler2D ColorMap;
	layout(binding = 1) uniform sampler2D NormalMap;
	layout(binding = 2) uniform sampler2D HeightMap;
	layout(binding = 3) uniform sampler2D SpecularMap;
	layout(binding = 4) uniform sampler2D EmissiveMap;
	layout(binding = 5) uniform sampler2D OpacityMap;

	uniform bool ColorMapIsUsed;
	uniform bool NormalMapIsUsed;
	uniform bool HeightMapIsUsed;
	uniform bool SpecularMapIsUsed;
	uniform bool EmissiveMapIsUsed;
	uniform bool OpacityMapIsUsed;
	uniform float TilingU = 1.0;
	uniform float TilingV = 1.0;
	uniform vec2 TilingUV = vec2(1.0);

	struct LightSource
	{
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		vec4 position;
		vec4 lookAt; // directional & spot
		vec2 attenuation; // point & spot
		float cutoff;     // spot
		float exponent;   // spot
	};
	const int MaxLights = 32;
	uniform LightSource Lights[MaxLights];
	uniform int TotalLights = 0;

	struct MaterialInfo
	{
		vec4 ambient;
		vec4 diffuse;
		vec4 specular;
		float shininess;
	};
	uniform MaterialInfo Material = MaterialInfo(
		vec4(0.3, 0.3, 0.3, 1.0),
		vec4(0.7, 0.7, 0.7, 1.0),
		vec4(0.5, 0.5, 0.5, 1.0),
		60.0
	);
)";