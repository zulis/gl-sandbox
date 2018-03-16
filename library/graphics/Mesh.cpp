#include "Mesh.h"
#include "system/Log.h"
#pragma region assimp
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#pragma endregion assimp
#include <unordered_map>

namespace library
{

namespace MeshData {
    enum class TextureType
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

    /*std::unordered_map<TextureType, std::string> TextureTypeName =
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
        };*/
};

//=========================================================================

class Mesh::Impl
{
public:
};

//=========================================================================

Mesh::Mesh()
    : impl{std::make_unique<Impl>()}
{
}

Mesh::~Mesh()
{
    note("Mesh released.");
}

void Mesh::fromFile(const std::string &fileName)
{
    unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs |
        aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_SplitLargeMeshes | aiProcess_SortByPType | aiProcess_TransformUVCoords;

    auto importer = std::unique_ptr<Assimp::Importer>(new Assimp::Importer());
    importer->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    auto scene = importer->ReadFile(fileName.c_str(), flags);

    if(scene) {
        note("Mesh loaded: {}", fileName);
        note("  Submeshes  : {}", scene->mNumMeshes);
        note("  Materials  : {}", scene->mNumMaterials);
        note("  Animations : {}", scene->mNumAnimations);

        const aiVector3D zero3D(0.0f, 0.0f, 0.0f);

        for(unsigned int m = 0; m < scene->mNumMeshes; ++m)
        {
            const aiMesh* mesh = scene->mMeshes[m];

            /* MeshDataGeometry meshDataGeometry;
            meshDataGeometry.materialIndex = mesh->mMaterialIndex;
            meshDataGeometry.indices = getIndices(mesh);
            meshDataGeometry.vertices = getVertices(mesh, scaleFactor);
            meshDataGeometry.normals = getNormals(mesh);
            meshDataGeometry.tangents = getTangents(mesh);
            meshDataGeometry.bitangents = getBitangents(mesh);
            meshDataGeometry.texCoords = getTexCoords(mesh);

            meshData.geometryVec.push_back(meshDataGeometry);*/
        }
    }
    else {
        error("Could not load mesh: {}", fileName);
    }

}

void Mesh::draw()
{
}
}
