#pragma once

#include <string>

class ShaderConstants
{
public:
	static const std::string VertexPosition;
	static const std::string VertexNormal;
	static const std::string VertexTangent;
	static const std::string VertexBitangent;
	static const std::string VertexTexCoord;
	static const std::string ProjectionMatrix;
	static const std::string ViewMatrix;
	static const std::string ModelMatrix;
	static const std::string ModelViewMatrix;
	static const std::string NormalMatrix;
	static const std::string MVP;
	static const std::string MaterialAmbient;
	static const std::string MaterialDiffuse;
	static const std::string MaterialSpecular;
	static const std::string MaterialShininess;
	static const std::string LightPosition;
	static const std::string LightAmbient;
	static const std::string LightDiffuse;
	static const std::string LightSpecular;
	//static const std::string LightIntensity;
	static const std::string DiffuseMap;
	static const std::string NormalMap;
	static const std::string SpecularMap;
	static const std::string HeightMap;
	static const std::string OpacityMap;
	static const std::string DiffuseMapIsUsed;
	static const std::string NormalMapIsUsed;
	static const std::string SpecularMapIsUsed;
	static const std::string HeightMapIsUsed;
	static const std::string OpacityMapIsUsed;
};

const std::string ShaderConstants::VertexPosition = std::string("VertexPosition");
const std::string ShaderConstants::VertexNormal = std::string("VertexNormal");
const std::string ShaderConstants::VertexTangent = std::string("VertexTangent");
const std::string ShaderConstants::VertexBitangent = std::string("VertexBitangent");
const std::string ShaderConstants::VertexTexCoord = std::string("VertexTexCoord");
const std::string ShaderConstants::ProjectionMatrix = std::string("ProjectionMatrix");
const std::string ShaderConstants::ViewMatrix = std::string("ViewMatrix");
const std::string ShaderConstants::ModelMatrix = std::string("ModelMatrix");
const std::string ShaderConstants::ModelViewMatrix = std::string("ModelViewMatrix");
const std::string ShaderConstants::NormalMatrix = std::string("NormalMatrix");
const std::string ShaderConstants::MVP = std::string("MVP");
const std::string ShaderConstants::MaterialAmbient = std::string("Material.ambient");
const std::string ShaderConstants::MaterialDiffuse = std::string("Material.diffuse");
const std::string ShaderConstants::MaterialSpecular = std::string("Material.specular");
const std::string ShaderConstants::MaterialShininess = std::string("Material.shininess");
const std::string ShaderConstants::LightPosition = std::string("Lights[#].position");
const std::string ShaderConstants::LightAmbient = std::string("Lights[#].ambient");
const std::string ShaderConstants::LightDiffuse = std::string("Lights[#].diffuse");
const std::string ShaderConstants::LightSpecular = std::string("Lights[#].specular");
//const std::string ShaderConstants::LightIntensity = std::string("Light.Intensity");
const std::string ShaderConstants::DiffuseMap = std::string("DiffuseMap");
const std::string ShaderConstants::NormalMap = std::string("NormalMap");
const std::string ShaderConstants::SpecularMap = std::string("SpecularMap");
const std::string ShaderConstants::HeightMap = std::string("HeightMap");
const std::string ShaderConstants::OpacityMap = std::string("OpacityMap");
const std::string ShaderConstants::DiffuseMapIsUsed = std::string("DiffuseMapIsUsed");
const std::string ShaderConstants::NormalMapIsUsed = std::string("NormalMapIsUsed");
const std::string ShaderConstants::SpecularMapIsUsed = std::string("SpecularMapIsUsed");
const std::string ShaderConstants::HeightMapIsUsed = std::string("HeightMapIsUsed");
const std::string ShaderConstants::OpacityMapIsUsed = std::string("OpacityMapIsUsed");