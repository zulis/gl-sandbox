#pragma once

#include <string>
#include <unordered_map>

enum TextureType
{
	//AmbientMap,
	ColorMap,
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
	EmissiveMap,
	Unknown
};

std::unordered_map<TextureType, std::string> TextureTypeName =
{
	//{ TextureType::AmbientMap, "AmbientMap" },
	{ TextureType::ColorMap, "ColorMap" },
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
	{ TextureType::EmissiveMap, "EmissiveMap" },
	{ TextureType::Unknown, "Unknown" },
};