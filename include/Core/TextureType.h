#pragma once

#include <string>
#include <map>

enum TextureType
{
	//AmbientMap,
	DiffuseMap,
	SpecularMap,
	//SpecularLevel,
	//Glossiness,
	//SelfIllumination,
	OpacityMap,
	//FilterColor,
	NormalMap,
	//Reflection,
	//Refraction,
	//Displacement,
	HeightMap,
	Unknown
};

std::map<TextureType, std::string> TextureTypeName =
{
	//{ TextureType::AmbientMap, "AmbientMap" },
	{ TextureType::DiffuseMap, "DiffuseMap" },
	{ TextureType::SpecularMap, "SpecularMap" },
	//{ TextureType::SpecularLevel, "SpecularLevel" },
	//{ TextureType::Glossiness, "Glossiness" },
	//{ TextureType::SelfIllumination, "SelfIllumination" },
	{ TextureType::OpacityMap, "OpacityMap" },
	//{ TextureType::FilterColor, "FilterColor" },
	{ TextureType::NormalMap, "NormalMap" },
	//{ TextureType::Reflection, "Reflection" },
	//{ TextureType::Refraction, "Refraction" },
	//{ TextureType::Displacement, "Displacement" },
	{ TextureType::HeightMap, "HeightMap" },
	{ TextureType::Unknown, "Unknown" },
};