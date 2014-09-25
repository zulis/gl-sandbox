#pragma once

#include <string>
#include <vector>
#include "Core/GL.h"
#include "Core/Color.h"
#include "Core/Math.h"
#include "Core/Texture.h"
#include "Core/TextureType.h"

struct MeshGeometry
{
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;
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