#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "core/GL.h"
#include "core/Color.h"
#include "core/Math.h"

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

struct MeshDataGeometry
{
	std::vector<vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<vec3> normals;
	std::vector<vec2> texCoords;
	std::vector<vec4> tangents;
	std::vector<vec3> bitangents;
	unsigned int materialIndex;
};

// struct MeshTexture
// {
// 	TextureType textureType;
// 	//TextureRef texture;
// 	std::string fileName;
// 	//GLuint textureID;
// 
// 	bool operator==(const MeshTexture& t) const
// 	{
// 		return t.textureType == textureType;
// 	}
// };

struct MeshDataMaterial
{
	Color ambient;
	Color diffuse;
	Color specular;
	Color emissive;
	Color transparent;
	//float shininess;
	std::map<TextureType, std::string> textureMap;

// 	TextureRef getTexture(TextureType type) const
// 	{
// 		MeshTexture mt;
// 		mt.type = type;
//
// 		auto it = std::find(textures.begin(), textures.end(), mt);
//
// 		if (it != textures.end())
// 			return (*it).texture;
// 		else
// 			return NULL;
// 	}
};

struct MeshData
{
	std::vector<MeshDataGeometry> geometryVec;
	std::map<unsigned int, MeshDataMaterial> materialMap;
};

//typedef std::vector<MeshPart> MeshData;