#pragma once

#include <string>
#include <map>

enum class TextureType
{
	AmbientMap,
	DiffuseMap,
	SpecularMap,
	SpecularLevel,
	Glossiness,
	SelfIllumination,
	OpacityMap,
	FilterColor,
	NormalMap,
	Reflection,
	Refraction,
	Displacement,
	Unknown
};

std::map<TextureType, std::string> TextureTypeName =
{
	{ TextureType::AmbientMap, "AmbientMap" },
	{ TextureType::DiffuseMap, "DiffuseMap" },
	{ TextureType::SpecularMap, "SpecularMap" },
	{ TextureType::SpecularLevel, "SpecularLevel" },
	{ TextureType::Glossiness, "Glossiness" },
	{ TextureType::SelfIllumination, "SelfIllumination" },
	{ TextureType::OpacityMap, "OpacityMap" },
	{ TextureType::FilterColor, "FilterColor" },
	{ TextureType::NormalMap, "NormalMap" },
	{ TextureType::Reflection, "Reflection" },
	{ TextureType::Refraction, "Refraction" },
	{ TextureType::Displacement, "Displacement" },
	{ TextureType::Unknown, "Unknown" },
};