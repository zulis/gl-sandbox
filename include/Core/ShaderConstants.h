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
	static const std::string MaterialKd;
	static const std::string MaterialKa;
	static const std::string MaterialKs;
	static const std::string MaterialShininess;
	static const std::string LightPosition;
	static const std::string LightIntensity;
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
const std::string ShaderConstants::MaterialKd = std::string("Material.Kd");
const std::string ShaderConstants::MaterialKa = std::string("Material.Ka");
const std::string ShaderConstants::MaterialKs = std::string("Material.Ks");
const std::string ShaderConstants::MaterialShininess = std::string("Material.Shininess");
const std::string ShaderConstants::LightPosition = std::string("Light.Position");
const std::string ShaderConstants::LightIntensity = std::string("Light.Intensity");