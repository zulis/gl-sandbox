#pragma once

#include <string>
#include <vector>
#include "Core/Color.h"
#include "Core/Math.h"
#include "Core/TextureType.h"

struct MeshGeometry
{
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;
	std::vector<glm::vec2> texCoords;
};

struct MeshTexture
{
	TextureType type;
	std::string fileName;
};

struct MeshMaterial
{
	Color diffuseColor;
	Color ambientColor;
	Color specularColor;
	Color emissiveColor;
	Color transparentColor;
	float shininess{ 0.5f };
	std::vector<MeshTexture> textures;
};

struct MeshPart
{
	MeshGeometry geometry;
	MeshMaterial material;
};

typedef std::vector<MeshPart> MeshData;