#pragma once

#include "core/BaseApp.h"
#include "GBuffer.h"

class Deferred1 : public BaseApp
{
public:
	Deferred1();
	~Deferred1();

	virtual void onInput(const Input &input) override;
	virtual void onUpdate(double deltaTime) override;
	virtual void onDraw() override;
	virtual void onResize(const unsigned int width, const unsigned int height) override;
private:
	Mesh mMesh;
	TextureID mTexD, mTexN, mTexS;
	ShaderID mShaderGeometry;
	GBuffer mGBuffer;
};

//=========================================================================
Deferred1::Deferred1() : BaseApp(1280, 720, WindowMode::Windowed)
{
	setTitle("Deferred rendering...");
	renderer.setCearColor(Color::gray(0.5f));

	if (!mGBuffer.init(getViewportWidth(), getViewportHeight()))
		quit();

	// Setup and compile our shaders
	mShaderGeometry = renderer.addShader(R"(
		[Vertex]
		#include "core"

		out vec2 TexCoord;
		out vec3 Position;
		out vec3 Normal;

		void main()
		{
			TexCoord = VertexTexCoord;
			vec4 position = ModelViewMatrix * vec4(VertexPosition, 1.0);
			Position = position.xyz;
			Normal = normalize(NormalMatrix * VertexNormal);
			gl_Position = ProjectionMatrix * position;
		}

		[Fragment]
		#include "core"

		in vec2 TexCoord;
		in vec3 Position;
		in vec3 Normal;

		layout (location = 0) out vec3 WorldPosOut;   
		layout (location = 1) out vec3 DiffuseOut;     
		layout (location = 2) out vec3 NormalOut;     
		layout (location = 3) out vec3 TexCoordOut;

		void main()
		{
			WorldPosOut = Position;					
			DiffuseOut  = texture(ColorMap, TexCoord).xyz;	
			NormalOut   = normalize(Normal);					
			TexCoordOut = vec3(TexCoord, 0.0);
		}
	)", Shader::SourceType::String);

	// Setup lights
	
	mMesh.loadFromFile("assets/models/leprechaun/leprechaun.fbx");
	mMesh.makeDrawable(renderer, mShaderGeometry);
	
	mTexD = renderer.addTexture("assets/models/leprechaun/leprechaun_d.png");
	mTexN = renderer.addTexture("assets/models/leprechaun/leprechaun_n.png");
	mTexS = renderer.addTexture("assets/models/leprechaun/leprechaun_s.png");

	camera.setPosition(0, 2, -10);
	camera.setLookAt(0, 4, 0);

	gl::enableCullFace(gl::CullFaceType::Back);
}

//=========================================================================
Deferred1::~Deferred1()
{
}

//=========================================================================
inline void Deferred1::onInput(const Input &input)
{
	if (input.isKeyDown(KEY_ESCAPE))
		quit();

	if (input.isKeyDown(KEY_LEFT_SHIFT))
		camera.setStrafeSpeed(0.4f);
	else
		camera.setStrafeSpeed(0.2f);

	if (input.isKeyDown(KEY_W))
		camera.move(Camera::FORWARD);
	else if (input.isKeyDown(KEY_S))
		camera.move(Camera::BACKWARD);

	if (input.isKeyDown(KEY_A))
		camera.move(Camera::LEFT);
	else if (input.isKeyDown(KEY_D))
		camera.move(Camera::RIGHT);

	if (input.isKeyDown(KEY_E))
		camera.move(Camera::UP);
	else if (input.isKeyDown(KEY_Q))
		camera.move(Camera::DOWN);

	if (input.getMouseScroolY() != 0)
		camera.move(input.getMouseScroolY() > 0 ? Camera::FORWARD : Camera::BACKWARD);

	if (input.isMouseDown(Input::MouseButton::Right))
	{
		hideMouse();
		camera.rotate(static_cast<float>(input.getMouseChangeX()), static_cast<float>(input.getMouseChangeY()));
	}
	else
		showMouse();
}

//=========================================================================
inline void Deferred1::onUpdate(double deltaTime)
{
}

//=========================================================================
inline void Deferred1::onDraw()
{
	// Geometry pass
	mGBuffer.bindForWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Transform transform;
	transform.setRotationX(-90);
	transform.setRotationY(180);
	transform.setScale(0.1f);

	renderer.setShader(mShaderGeometry);
	renderer.setShaderUniform(ShaderConstants::ProjectionMatrix, camera.getProjectionMatrix());
	renderer.setShaderUniform(ShaderConstants::ModelViewMatrix, camera.getViewMatrix() * transform.getMatrix());

	auto mv = camera.getViewMatrix() * transform.getMatrix();
	renderer.setShaderUniform(ShaderConstants::NormalMatrix, mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));

	// Color map
	renderer.setTexture(mTexD, 0);
	renderer.setShaderUniform(ShaderConstants::ColorMap, 0);

	// Draw mesh
	mMesh.draw();

	// Light pass
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mGBuffer.bindForReading();

	GLint halfWidth = (GLint)(getViewportWidth() / 2.0f);
	GLint halfHeight = (GLint)(getViewportHeight() / 2.0f);

	mGBuffer.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
	glBlitFramebuffer(0, 0, getViewportWidth(), getViewportHeight(), 0, halfHeight, halfWidth, getViewportHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

	mGBuffer.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
	glBlitFramebuffer(0, 0, getViewportWidth(), getViewportHeight(), halfWidth, halfHeight, getViewportWidth(), getViewportHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

	mGBuffer.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
	glBlitFramebuffer(0, 0, getViewportWidth(), getViewportHeight(), 0, 0, halfWidth, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	mGBuffer.setReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
	glBlitFramebuffer(0, 0, getViewportWidth(), getViewportHeight(), halfWidth, 0, getViewportWidth(), halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

//=========================================================================
inline void Deferred1::onResize(const unsigned int width, const unsigned int height)
{
	camera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
	mGBuffer.resize(width, height);
}
