#pragma once

#include <iostream>
#include <mutex>
#include "core/Game.h"
#include "core/Camera.h"
//#include "core/Shape.h"
//#include "core/Transform3d.h"
#include "core/Resource.h"
//#include "core/Image.h"
#include "core/Texture.h"
#include "core/Shader.h"
#include "core/MeshDataLoader.h"
#include "core/Geometry.h"
#include "core/Transform.h"
#include "core/FileMonitor.h"
#include "core/Listeners.h"
#include "core/Ui.h"
#include "core/CameraControllers.h"

class Deferred : public State, public FileMonitorListener
{
public:
    Deferred() {};
    ~Deferred() {};

    virtual void setup();
    virtual void cleanup();
    virtual void input(Input& input);
    virtual void update(double elapsedTime);
    virtual void draw();
    virtual void resize(unsigned int width, unsigned int height);

    virtual void onFileMonitorFileChange(const std::string& fileName);

private:
    CameraPtr mCamera;
    float mStrafeSpeed{ 0.2f };
    float mStrafeFastSpeed{ 0.4f };

    //ShaderRef mShader;
    //GeometryRef mShape;
    //Transform3D mTransform;
    //TextureRef mColorTex;
    ShaderPtr mShader;
    std::vector<std::shared_ptr<Geometry>> mGeometryVec;
	std::map<unsigned int, TextureRef> mTextureMap;
    FileMonitorRef mFileMonitor;
    vec3 mMousePickPos;
	static bool mShowColorMap;

    void drawUI();
};

bool Deferred::mShowColorMap = true;

//=========================================================================
void Deferred::setup()
{
    //auto a1 = Resource::get<Texture>("assets\\textures\\default\\default_d.png");
    //auto a2 = Resource::get<Texture>("assets\\textures\\default\\default_d.png");
    //auto a3 = Resource::get<Texture>("assets\\textures\\default\\default_d.png");

    //auto b1 = Resource::get<Texture>("assets\\textures\\default\\default_n.png");
    //auto b2 = Resource::get<Texture>("assets\\textures\\default\\default_n.png");
    //auto b3 = Resource::get<Texture>("assets\\textures\\default\\default_n.png");

    //auto a1 = Resource::get<Texture>(Color::red());
    //auto a2 = Resource::get<Texture>(Color::red());
    //auto a3 = Resource::get<Texture>(Color::red());

    //auto b1 = Resource::get<Texture>(Color::blue());
    //auto b2 = Resource::get<Texture>(Color::blue());
    //auto b3 = Resource::get<Texture>(Color::blue());

    //auto sh1 = Shared<Shader>::create();
    //sh1->loadFromFile("assets\\shaders\\basic.vert", ShaderType::Vertex);
    //sh1->loadFromFile("assets\\shaders\\basic.frag", ShaderType::Fragment);
    //sh1->link();

    mFileMonitor = FileMonitor::create();
    mFileMonitor->addFile("assets\\shaders\\basic.vert");
    mFileMonitor->addFile("assets\\shaders\\basic.frag");
    mFileMonitor->addListener(this);

    mShader = Shader::create();
    mShader->loadFromFile("assets\\shaders\\basic.vert", ShaderType::Vertex);
    mShader->loadFromFile("assets\\shaders\\basic.frag", ShaderType::Fragment);
    mShader->link();

	//auto mesh = Resource::get<MeshDataLoader>("assets\\models\\box\\box.obj");
	//auto mesh = std::shared_ptr<MeshDataLoader>(new MeshDataLoader("assets\\models\\sponza\\sponza.obj", 0.5f));
	auto mesh = std::shared_ptr<MeshDataLoader>(new MeshDataLoader("assets\\models\\leprechaun\\leprechaun.fbx"));
	//auto mesh = Resource::get<MeshDataLoader>("assets\\models\\robot\\robot.obj");

	auto meshData = mesh->getMeshData();

	auto idx = 0;
    for (const auto& geo : meshData.geometryVec)
    {
        auto geometry = Shared<Geometry>::create();
		geometry->setIndices(geo.indices);
        geometry->setVertices(geo.vertices);
		geometry->setNormals(geo.normals);
        geometry->setTexCoords(geo.texCoords);
        geometry->setTangents(geo.tangents);
        geometry->setBitangents(geo.bitangents);
        geometry->prepare(*mShader);

		mGeometryVec.push_back(geometry);

		auto colorTex = meshData.materialMap[geo.materialIndex].textureMap[TextureType::ColorMap];
		//note(colorTex.c_str());

		if (colorTex.size() > 0)
			mTextureMap[idx++] = Texture::create(("assets\\models\\leprechaun\\" + colorTex).c_str());
    }


    mCamera = Camera::create();
    mCamera->setRotateSpeed(0.002f);
    mCamera->setStrafeSpeed(mStrafeSpeed);
    mCamera->setPosition(vec3(3.5, 2.2, -8.8));
    //mCamera->setDirection(vec3(-0.023f, 0.419f, 0.907f));
    mCamera->setDirection(vec3(-0.4, 0.2, 0.9));
    //mCamera->setLookAt(vec3(0, 0, 0));

    /*
    mShader = Shader::create("assets/shaders/basic");
    mShape = Shape::createCube();
    mColorTex = Texture::create("assets/textures/default/default_d.png");*/
}

//=========================================================================
void Deferred::cleanup()
{

}

//=========================================================================
void Deferred::input(Input& input)
{
    CameraController::flyController(mCamera, input);

    if (input.isKeyDown(KEY_ESCAPE))
        quit();

    mMousePickPos = mCamera->pick(input.getMousePosition());
}

//=========================================================================
void Deferred::update(double elapsedTime)
{
    mFileMonitor->update();
}

//=========================================================================
void Deferred::draw()
{
    gl::enableCullFace(gl::CullFaceType::Back);

    Transform transform;
    transform.setRotationX(-90);
    transform.setRotationY(180);
    transform.setScale(0.1);

    mShader->bind();

    mShader->setUniform(ShaderConstants::ProjectionMatrix, mCamera->getProjectionMatrix());
    //mShader->setUniform(ShaderConstants::ViewMatrix,  mCamera->getViewMatrix());
    //mShader->setUniform(ShaderConstants::ModelMatrix, transform.getMatrix());
    mShader->setUniform(ShaderConstants::ModelViewMatrix, mCamera->getViewMatrix() * transform.getMatrix());
    mShader->setUniform(ShaderConstants::MVP, mCamera->getProjectionMatrix() *  mCamera->getViewMatrix() * transform.getMatrix());

    auto mv = mCamera->getViewMatrix() * transform.getMatrix();
    mShader->setUniform(ShaderConstants::NormalMatrix, mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

    //mShader->setUniform(ShaderConstants::MVP, mCamera->get ->getViewMatrix() * transform.getMatrix());

	auto idx = 0;

    for (const auto& geometry : mGeometryVec)
    {
		if (mTextureMap[idx] && mShowColorMap)
		{
			mTextureMap[idx]->bind();
			mShader->setUniform(ShaderConstants::ColorMap, 0);
			mShader->setUniform(ShaderConstants::ColorMapIsUsed, true);
		}
		else
			mShader->setUniform(ShaderConstants::ColorMapIsUsed, false);

		idx++;

        geometry->draw();
    }

    mShader->unbind();

    //mColorTex->bind();
    //mShader->bind();

    ////mShader->setUniform(ShaderConstants::ProjectionMatrix, mCamera->getProjectionMatrix());
    ////mShader->setUniform(ShaderConstants::ViewMatrix, mCamera->getViewMatrix());
    ////mShader->setUniform(ShaderConstants::ModelMatrix, mTransform.getMatrix());
    ////mShader->setUniform(ShaderConstants::ModelViewMatrix, mCamera->getViewMatrix() * mTransform.getMatrix());
    //mShader->setUniform(ShaderConstants::MVP, mCamera->getProjectionMatrix() * mCamera->getViewMatrix() * mTransform.getMatrix());
    //mShader->setUniform(ShaderConstants::ColorMapIsUsed, true);
    ////mShader->setUniform(ShaderConstants::ColorMap, 0);

    ////auto mv = mCamera->getViewMatrix() * mTransform.getMatrix();
    ////mShader->setUniform(ShaderConstants::NormalMatrix, mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

    //mShape->draw(*mShader);
    //mShader->unbind();

    //mColorTex->unbind();

    gl::disableCullFace();

    drawUI();
}

//=========================================================================
void Deferred::resize(unsigned int width, unsigned int height)
{
    mCamera->setAspectRatio((float)width / height);
}

//=========================================================================
void Deferred::onFileMonitorFileChange(const std::string& fileName)
{
    mShader->reload();
}

//=========================================================================
void Deferred::drawUI()
{
    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoResize;

    ui::SetNextWindowPos(ImVec2(10, 10), ImGuiSetCond_FirstUseEver); // ImGuiSetCond_Always/*ImGuiSetCond_FirstUseEver*/);
    ui::Begin("Test", (bool*)true, ImVec2(500, 1000), 0.0f, flags);

    static bool showHelp = true;
    if (ui::Button(showHelp ? "Hide help" : "Show help"))
        showHelp ^= 1;

    ui::SameLine();
    if (ui::Button("Close"))
        quit();

    if (showHelp)
    {
        ui::Separator();
        ui::Text("Frame time %.3f ms | FPS: %.1f", 1000.0f / ui::GetIO().Framerate, ui::GetIO().Framerate);

        static int item = 1;
        ui::Combo("combo", &item, "aaaa\0bbbb\0cccc\0dddd\0eeee\0\0");

        ui::Text("Cam position:  %s", to_string(mCamera->getPosition()).c_str());
        ui::Text("Cam direction: %s", to_string(mCamera->getDirection()).c_str());
        //ui::Text("Mouse pick:    %s", to_string(mMousePickPos).c_str());
		ui::Checkbox("Show color map", &mShowColorMap);
        
    }

    ui::End();

    //ui::ShowTestWindow(NULL);
}
