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
	static const std::string LightIntensity;
	static const std::string TexDiffuse;
	static const std::string TexNormal;
	static const std::string TexSpecular;
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
const std::string ShaderConstants::LightPosition = std::string("Light.Position");
const std::string ShaderConstants::LightIntensity = std::string("Light.Intensity");
const std::string ShaderConstants::TexDiffuse = std::string("TexDiffuse");
const std::string ShaderConstants::TexNormal = std::string("TexNormal");
const std::string ShaderConstants::TexSpecular = std::string("TexSpecular");