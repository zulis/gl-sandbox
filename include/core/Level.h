#pragma once

#include <memory>
#include <vector>
#include <fstream>
#include <json.h>
#include "core/Mesh.h"
#include "core/DirectionalLight.h"
#include "core/PointLight.h"
#include "core/Camera.h"
#include "core/Math.h"
#include "core/Log.h"
#include "core/StringUtils.h"

struct LevelMeshData
{
	std::string name;
	std::string fileName;
	glm::ivec2 textureScale;
	bool visible;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

struct LevelLightData
{
	LightType type;
	bool visible;
	glm::vec3 position;
	glm::vec3 direction;
	float radius;
	Color color;
};

struct LevelData
{
	std::vector<LevelMeshData> meshList;
	std::vector<LevelLightData> lightList;
};

typedef std::shared_ptr<class Level> LevelRef;

class Level
{
public:
	static LevelRef create();
	Level();
	~Level();

	void setModelFormat(const std::string& modelFormat);
	void loadFromFile(const std::string& fileName);
	void update(double elapsedTime);
	void draw(const CameraRef& camera);

	const glm::vec3 getCamPosition() const;
	const glm::vec3 getCamLookAt() const;

private:
	std::vector<MeshRef> mMeshList;
	glm::vec3 mCamPosition{ glm::vec3() };
	glm::vec3 mCamLookAt{ glm::vec3() };
	std::string mModelFormat;

	LevelMeshData getMeshData(const Json::Value& data);
	LevelLightData getPointLightData(const Json::Value& data);
	LevelLightData getDirectionalLightData(const Json::Value& data);
};

//=========================================================================
LevelRef Level::create()
{
	return LevelRef(new Level);
}

//=========================================================================
Level::Level()
{
}

//=========================================================================
Level::~Level()
{
}

//=========================================================================
void Level::setModelFormat(const std::string& modelFormat)
{
	mModelFormat = modelFormat;
}

//=========================================================================
void Level::loadFromFile(const std::string& fileName)
{
	Json::Value root;
	Json::Reader reader;
	std::ifstream stream(fileName);

	bool parsingSuccessful = reader.parse(stream, root, false);
	stream.close();

	if(!parsingSuccessful)
	{
		logError(reader.getFormattedErrorMessages().c_str());
		return;
	}

	LevelData levelData;

	const Json::Value scene = root["Scene"];
	const Json::Value objects = scene["Objects"];
	const Json::Value lights = scene["Lights"];
	const Json::Value editor = scene["Editor"];

	if (objects["Object"].isArray())
	{
		for (Json::Value::iterator it = objects["Object"].begin(); it != objects["Object"].end(); ++it)
		{
			auto data = (*it);
			levelData.meshList.push_back(getMeshData(data));
		}
	}
	else if (objects["Object"].size() > 0)
	{
		auto data = objects["Object"];
		levelData.meshList.push_back(getMeshData(data));
	}

	if (lights["Directional"].isArray())
	{
		for (Json::Value::iterator it = lights["Directional"].begin(); it != lights["Directional"].end(); ++it)
		{
			auto data = (*it);
			levelData.lightList.push_back(getDirectionalLightData(data));
		}
	}
	else if (lights["Directional"].size() > 0)
	{
		auto data = lights["Directional"];
		levelData.lightList.push_back(getDirectionalLightData(data));
	}

	if (lights["Point"].isArray())
	{
		for (Json::Value::iterator it = lights["Point"].begin(); it != lights["Point"].end(); ++it)
		{
			auto data = (*it);
			levelData.lightList.push_back(getPointLightData(data));
		}
	}
	else if (lights["Point"].size() > 0)
	{
		auto data = lights["Point"];
		levelData.lightList.push_back(getPointLightData(data));
	}

	for(Json::Value::iterator it = lights["Spot"].begin(); it != lights["Spot"].end(); ++it)
	{
	}

	for(auto& meshData : levelData.meshList)
	{
		if (!meshData.visible)
			continue;

		meshData.position.z = -meshData.position.z;

		auto mesh = Mesh::create();
		mesh->setFrustumCulling(true);
		mesh->loadFromFile(meshData.fileName);
		mesh->setPosition(meshData.position);
		mesh->setRotation(meshData.rotation);
		mesh->setScale(meshData.scale);

		auto material = mesh->getMaterial();
		material->setTilingU(meshData.textureScale.x);
		material->setTilingV(meshData.textureScale.y);

		for(auto& lightData : levelData.lightList)
		{
			if (!lightData.visible)
				continue;

			lightData.position.z = -lightData.position.z;

			switch(lightData.type)
			{
				case LightType::Directional:
					{
						auto light = DirectionalLight::create();
						light->setPosition(lightData.position);
						light->setAmbient(lightData.color);
						light->setDiffuse(lightData.color);
						material->addLight(*light);
						break;
					}

				case LightType::Point:
					{
						auto light = PointLight::create();
						light->setPosition(lightData.position);
						light->setAmbient(lightData.color);
						light->setDiffuse(lightData.color);
						light->setAttenuation(glm::vec2(0.0f, lightData.radius));
						material->addLight(*light);
						break;
					}
			}
		}

		mMeshList.push_back(mesh);
	}

	auto camPosition = StringUtils::toVec3(editor["CamPosition"].asString());
	camPosition.z = -camPosition.z;
	mCamPosition = camPosition;
	auto camLookAt = StringUtils::toVec3(editor["CamLookAt"].asString());;
	camLookAt.z = -camLookAt.z;
	mCamLookAt = camLookAt;

	return;
}

//=========================================================================
void Level::update(double elapsedTime)
{

}

//=========================================================================
void Level::draw(const CameraRef& camera)
{
	gl::enableCullFace(gl::CullFaceType::Back);
	for(auto& mesh : mMeshList)
	{
		mesh->draw(camera);
	}
	gl::disableCullFace();
}

//=========================================================================
const glm::vec3 Level::getCamPosition() const
{
	return mCamPosition;
}

//=========================================================================
const glm::vec3 Level::getCamLookAt() const
{
	return mCamLookAt;
}

//=========================================================================
LevelMeshData Level::getMeshData(const Json::Value& data)
{
	LevelMeshData meshData;
	meshData.name = data["Name"].asString();
	meshData.fileName = data["FileName"].asString();

	if (mModelFormat != std::string())
		meshData.fileName = StringUtils::cutTail(meshData.fileName, ".") + mModelFormat;

	meshData.textureScale = StringUtils::toVec2(data["ScaleTexture"].asString());
	meshData.visible = StringUtils::toBool(data["Visible"].asString());
	meshData.position = StringUtils::toVec3(data["Position"].asString());
	meshData.rotation = StringUtils::toVec3(data["Rotation"].asString());
	meshData.scale = StringUtils::toVec3(data["Scale"].asString());
	return meshData;
}

//=========================================================================
LevelLightData Level::getPointLightData(const Json::Value& data)
{
	LevelLightData lightData;
	lightData.type = LightType::Point;
	lightData.visible = StringUtils::toBool(data["Visible"].asString());
	lightData.position = StringUtils::toVec3(data["Position"].asString());
	lightData.direction = glm::vec3();
	lightData.radius = StringUtils::toFloat(data["Radius"].asString());
	lightData.color = Color(StringUtils::toVec3(data["Color"].asString()) / 255.0f);
	return lightData;
}

//=========================================================================
LevelLightData Level::getDirectionalLightData(const Json::Value& data)
{
	LevelLightData lightData;
	lightData.type = LightType::Directional;
	lightData.visible = StringUtils::toBool(data["Visible"].asString());
	lightData.position = StringUtils::toVec3(data["Position"].asString());
	lightData.direction = StringUtils::toVec3(data["Direction"].asString());
	lightData.radius = 0.0f;
	lightData.color = Color(StringUtils::toVec3(data["Color"].asString()) / 255.0f);
	return lightData;
}
