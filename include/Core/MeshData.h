#pragma once

#include <string>
#include <vector>
#include "Core/Math.h"
#include "Core/TextureType.h"

struct MeshGeometry
{
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec4> tangents;
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
	glm::vec3 Kd{ glm::vec3(0.8f, 0.8f, 0.8f) };
	glm::vec3 Ka{ glm::vec3(0.8f, 0.8f, 0.8f) };
	glm::vec3 Ks{ glm::vec3(0.1f, 0.1f, 0.1f) };
	float Shininess{ 0.5f };
	std::vector<MeshTexture> textures;
};

struct MeshPart
{
	MeshGeometry geometry;
	MeshMaterial material;
};

typedef std::vector<MeshPart> MeshData;