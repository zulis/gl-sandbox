#pragma once

#include <memory>
#include <string>
#include <vector>
#include <algorithm>


#include "Core/MeshLoader.h"
#include "Core/Geometry.h"
#include "Core/Drawable.h"
#include "Core/Transform3d.h"
#include "Core/Texture.h"

typedef std::shared_ptr<class Mesh> MeshRef;

class Mesh : public Drawable, public Transform3D
{
	public:
		static MeshRef create(const std::string& fileName, float scaleFactor = 1.0f);

		Mesh(const std::string& fileName, float scaleFactor = 1.0f);
		~Mesh();

		void draw(const CameraRef& camera);
		
		const std::vector<Material::GeometryMaterial>& getGeomMaterial();
		void setTexturePath(const std::string& texturePath);

	protected:
		virtual void setGeometry(std::vector<GeometryRef>* geometries);

private:
	MeshLoaderRef mMeshLoader;
	std::vector<GeometryRef> mGeometries;
	void load(const std::string& fileName, float scaleFactor);
	std::string mTexturePath { std::string() };
	std::vector<Material::GeometryMaterial> mGeometriesMaterials;
	//std::string getFileName(std::string& pathName);
	//void parseNode(const aiNode* node);

};

//=========================================================================
MeshRef Mesh::create(const std::string& fileName, float scaleFactor)
{
	return MeshRef(new Mesh(fileName, scaleFactor));
}

//=========================================================================
Mesh::Mesh(const std::string& fileName, float scaleFactor)
{
	mTexturePath = fileName;

	const size_t idx = mTexturePath.find_last_of("\\/");

	if(std::string::npos != idx)
		mTexturePath.erase(idx, mTexturePath.length() - idx);

	load(fileName, scaleFactor);
}

//=========================================================================
Mesh::~Mesh()
{
}

//=========================================================================
void Mesh::load(const std::string& fileName, float scaleFactor)
{
	mMeshLoader = MeshLoader::create();
	MeshData meshData = mMeshLoader->loadFromFile(fileName, scaleFactor);

	for each (auto meshPartData in meshData)
	{
		auto geometry = GeometryRef(new Geometry);
		geometry->setDrawType(Geometry::TRIANGLES);
		geometry->setVertices(meshPartData.geometry.vertices);
		geometry->setIndices(meshPartData.geometry.indices);
		geometry->setNormals(meshPartData.geometry.normals);
		geometry->setTangents(meshPartData.geometry.tangents);
		geometry->setBitangents(meshPartData.geometry.bitangents);
		geometry->setTexCoords(meshPartData.geometry.texCoords);
		mGeometries.push_back(geometry);
	}
}

//=========================================================================
void Mesh::setGeometry(std::vector<GeometryRef>* geometries)
{
	for(auto& geometry : mGeometries)
	{
		geometries->push_back(geometry);
	}
}

//=========================================================================
void Mesh::draw(const CameraRef& camera)
{
	Drawable::draw(*camera.get(), getMatrix());
}

//=========================================================================
void Mesh::setTexturePath(const std::string& texturePath)
{
	mTexturePath = texturePath;
}

//=========================================================================
const std::vector<Material::GeometryMaterial>& Mesh::getGeomMaterial()
{
	Texture::Format format;
	format.setFlipped(true);

	for(auto& gm : mGeometriesMaterials)
	{
		for(auto& t : gm.textures)
		{
			t.fileName = mTexturePath + "/" + t.fileName;
			t.texture = Texture::create(t.fileName, format);
		}
	}

	return mGeometriesMaterials;
}