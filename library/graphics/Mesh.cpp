#include "Mesh.h"
#include "system/Log.h"
#include "math/Math.h"
#include "graphics/Geometry.h"
#pragma region assimp
#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#pragma endregion assimp
#include <vector>

namespace library
{

namespace MeshData
{
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
    std::vector<std::unique_ptr<Geometry>> geometryVec;
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

    if (scene) {
        note("Mesh loaded: {}", fileName);
        note("  Submeshes  : {}", scene->mNumMeshes);
        note("  Materials  : {}", scene->mNumMaterials);
        note("  Animations : {}", scene->mNumAnimations);

        // https://github.com/TomasKimer/gles3mark/blob/master/gles3mark/sceneimporter.cpp

        for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
            auto geometry = std::make_unique<Geometry>();
            const aiMesh *aMesh = scene->mMeshes[m];

            if (aMesh->HasPositions()) {
                // vertices
                std::vector<glm::vec3> vertices(aMesh->mNumVertices);
                std::memcpy(&vertices[0].x, &aMesh->mVertices[0], aMesh->mNumVertices * sizeof(glm::vec3));
                geometry.get()->addVertices(vertices);

                // faces
                /*std::vector<glm::ivec3> faces(aMesh->mNumFaces);
                for (unsigned int j = 0; j < aMesh->mNumFaces; ++j) {
                    faces[j] = glm::make_vec3(aMesh->mFaces[j].mIndices);   //assert(aMesh->mFaces[0].mNumIndices == 3);
                }*/
                std::vector<unsigned int> indices;

                for (unsigned int i = 0; i < aMesh->mNumFaces; ++i) {
                    aiFace aiface = aMesh->mFaces[i];
                    unsigned numIndices = aiface.mNumIndices;
                    assert(numIndices <= 3);

                    for (int n = 0; n < 3; ++n) {
                        if (numIndices == 2 && n == 2) {
                            indices.push_back(aiface.mIndices[1]);
                        }
                        else {
                            indices.push_back(aiface.mIndices[n]);
                        }
                    }
                }
                geometry.get()->addIndices(indices);
            }

            // normals
            if (aMesh->HasNormals()) {
                std::vector<glm::vec3> normals(aMesh->mNumVertices, glm::vec3(0));
                std::memcpy(&normals[0].x, &aMesh->mNormals[0], aMesh->mNumVertices * sizeof(glm::vec3));
                geometry.get()->addNormals(normals);
            }

            // tangents (and bitangents?)
            if (aMesh->HasTangentsAndBitangents()) {
                std::vector<glm::vec3> tangents(aMesh->mNumVertices, glm::vec3(0));
                std::vector<glm::vec3> bitangents(aMesh->mNumVertices, glm::vec3(0));
                std::memcpy(&tangents[0].x, &aMesh->mTangents[0], aMesh->mNumVertices * sizeof(glm::vec3));
                std::memcpy(&bitangents[0].x, &aMesh->mBitangents[0], aMesh->mNumVertices * sizeof(glm::vec3));
                geometry.get()->addTangents(tangents);
                geometry.get()->addBitangents(bitangents);
            }

            // texture coordinates
            int texLevel = 0;
            if (aMesh->HasTextureCoords(texLevel)) {
                std::vector<glm::vec2> texCoords(aMesh->mNumVertices, glm::vec2(0));
                for (unsigned int j = 0; j < aMesh->mNumVertices; ++j) {
                    texCoords[j] = glm::make_vec2(&aMesh->mTextureCoords[texLevel][j].x);
                }
                geometry.get()->addTexCoords(texCoords);
            }

            impl->geometryVec.push_back(std::move(geometry));

            //const aiMesh* mesh = scene->mMeshes[m];

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
    for (auto &geometry: impl->geometryVec) {
        geometry.get()->draw();
    }
}
}
