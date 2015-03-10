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

struct MeshGeometry
{
	std::vector<vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<vec3> normals;
	std::vector<vec2> texCoords;
	std::vector<vec4> tangents;
	std::vector<vec3> bitangents;
	unsigned int materialIndex;
};

struct MeshTexture
{
	TextureType textureType;
	//TextureRef texture;
	std::string fileName;
	//GLuint textureID;

	bool operator==(const MeshTexture& t) const
	{
		return t.textureType == textureType;
	}
};

struct MeshMaterial
{
	Color ambient;
	Color diffuse;
	Color specular;
	//Color emissive;
	//Color transparent;
	float shininess;
	std::vector<MeshTexture> textures;

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

struct MeshPart
{
	MeshGeometry geometry;
	MeshMaterial material;
};

typedef std::vector<MeshPart> MeshData;

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>
#include "../external/src/tinyobjloader/tiny_obj_loader.h"

class MeshDataLoader
{
public:
	static MeshData loadFromFile(const char* fileName, float scaleFactor = 1.0f)
	{
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err = tinyobj::LoadObj(shapes, materials, fileName);

		MeshData result;

		for (const auto& s : shapes)
		{
			MeshGeometry mg;
			mg.indices = s.mesh.indices;

			for (int i = 0; i < s.mesh.positions.size(); i = i + 3)
				mg.vertices.push_back(vec3(s.mesh.positions[i], s.mesh.positions[i + 1], s.mesh.positions[i + 2]));

			for (int i = 0; i < s.mesh.normals.size(); i = i + 3)
				mg.normals.push_back(vec3(s.mesh.normals[i], s.mesh.normals[i + 1], s.mesh.normals[i + 2]));

			for (int i = 0; i < s.mesh.texcoords.size(); i = i + 2)
				mg.texCoords.push_back(vec2(s.mesh.texcoords[i], s.mesh.texcoords[i + 1]));

			MeshPart mp;
			mp.geometry = mg;

			result.push_back(mp);
		}

		return result;
	}
};