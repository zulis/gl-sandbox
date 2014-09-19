#pragma once

#include <string>
#include <map>

enum class TextureType
{
	AmbientColor,
	DiffuseColor,
	SpecularColor,
	SpecularLevel,
	Glossiness,
	SelfIllumination,
	Opacity,
	FilterColor,
	Bump,
	Reflection,
	Refraction,
	Displacement,
	Unknown
};

std::map<TextureType, std::string> TextureTypeName =
{
	{ TextureType::AmbientColor, "AmbientColor" },
	{ TextureType::DiffuseColor, "DiffuseColor" },
	{ TextureType::SpecularColor, "SpecularColor" },
	{ TextureType::SpecularLevel, "SpecularLevel" },
	{ TextureType::Glossiness, "Glossiness" },
	{ TextureType::SelfIllumination, "SelfIllumination" },
	{ TextureType::Opacity, "Opacity" },
	{ TextureType::FilterColor, "FilterColor" },
	{ TextureType::Bump, "Bump" },
	{ TextureType::Reflection, "Reflection" },
	{ TextureType::Refraction, "Refraction" },
	{ TextureType::Displacement, "Displacement" },
	{ TextureType::Unknown, "Unknown" },
};