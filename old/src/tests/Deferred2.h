#pragma once

#include "core/BaseApp.h"
#include "GBuffer2.h"
#include "Light.h"

class Deferred2 : public BaseApp
{
public:
	Deferred2();
	~Deferred2();

	virtual void onInput(const Input &input) override;
	virtual void onUpdate(double deltaTime) override;
	virtual void onDraw() override;
	virtual void onResize(const unsigned int width, const unsigned int height) override;
private:
	Mesh mMesh;
	TextureID mTexD, mTexN, mTexS;
	ShaderID mShaderGeometry;
	ShaderID mShaderDirectionalLight;
	ShaderID mShaderPointLight;
	GBuffer2 mGBuffer;
	SpotLight mSpotLight;
	DirectionalLight mDirLight;
	PointLight mPointLight[3];
};

//=========================================================================
Deferred2::Deferred2() : BaseApp(1280, 720, WindowMode::Windowed)
{
	setTitle("Deferred rendering...");
	renderer.setCearColor(Color::gray(0.5f));

	if (!mGBuffer.init(getViewportWidth(), getViewportHeight()))
		quit();

	// Setup lights
	mSpotLight.DiffuseIntensity = 0.9f;
	mSpotLight.Color = COLOR_WHITE;
	mSpotLight.Attenuation.Linear = 0.01f;
	mSpotLight.Position = glm::vec3(-20.0, 20.0, 5.0f);
	mSpotLight.Direction = glm::vec3(1.0f, -1.0f, 0.0f);
	mSpotLight.Cutoff = 20.0f;

	mDirLight.AmbientIntensity = 0.1f;
	mDirLight.Color = COLOR_CYAN;
	mDirLight.DiffuseIntensity = 0.5f;
	mDirLight.Direction = glm::vec3(1.0f, 0.0f, 0.0f);

	mPointLight[0].DiffuseIntensity = 0.2f;
	mPointLight[0].Color = COLOR_GREEN;
	mPointLight[0].Position = glm::vec3(0.0f, 1.5f, 5.0f);
	mPointLight[0].Attenuation.Constant = 0.0f;
	mPointLight[0].Attenuation.Linear = 0.0f;
	mPointLight[0].Attenuation.Exp = 0.3f;

	mPointLight[1].DiffuseIntensity = 0.2f;
	mPointLight[1].Color = COLOR_RED;
	mPointLight[1].Position = glm::vec3(2.0f, 0.0f, 5.0f);
	mPointLight[1].Attenuation.Constant = 0.0f;
	mPointLight[1].Attenuation.Linear = 0.0f;
	mPointLight[1].Attenuation.Exp = 0.3f;

	mPointLight[2].DiffuseIntensity = 0.2f;
	mPointLight[2].Color = COLOR_BLUE;
	mPointLight[2].Position = glm::vec3(0.0f, 0.0f, 3.0f);
	mPointLight[2].Attenuation.Constant = 0.0f;
	mPointLight[2].Attenuation.Linear = 0.0f;
	mPointLight[2].Attenuation.Exp = 0.3f;

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

	renderer.setShader(mShaderGeometry);
	renderer.setShaderUniform(ShaderConstants::ColorMap, 0);

	mShaderDirectionalLight = renderer.addShader(R"(
		[Vertex]
		#include "core"

		void main()
		{
			vec4 position = ModelViewMatrix * vec4(VertexPosition, 1.0);          
			gl_Position = ProjectionMatrix * position;
		}

		[Fragment]
		#include "core"

		struct BaseLight
		{
			vec3 Color;
			float AmbientIntensity;
			float DiffuseIntensity;
		};

		struct DirectionalLight
		{
			BaseLight Base;
			vec3 Direction;
		};

		struct Attenuation
		{
			float Constant;
			float Linear;
			float Exp;
		};

		struct PointLight
		{
			BaseLight Base;
			vec3 Position;
			Attenuation Atten;
		};

		struct SpotLight
		{
			PointLight Base;
			vec3 Direction;
			float Cutoff;
		};

		uniform sampler2D gPositionMap;
		uniform sampler2D gColorMap;
		uniform sampler2D gNormalMap;
		uniform DirectionalLight gDirectionalLight;
		uniform PointLight gPointLight;
		uniform SpotLight gSpotLight;
		uniform vec3 gEyeWorldPos;
		uniform float gMatSpecularIntensity;
		uniform float gSpecularPower;
		uniform int gLightType;
		uniform vec2 gScreenSize;

		vec4 CalcLightInternal(BaseLight Light,
							   vec3 LightDirection,
							   vec3 WorldPos,
							   vec3 Normal)
		{
			vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0);
			float DiffuseFactor = dot(Normal, -LightDirection);

					vec4 DiffuseColor  = vec4(0, 0, 0, 0);
			vec4 SpecularColor = vec4(0, 0, 0, 0);

					if (DiffuseFactor > 0.0) {
				DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0);

						vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos);
				vec3 LightReflect = normalize(reflect(LightDirection, Normal));
				float SpecularFactor = dot(VertexToEye, LightReflect);        
				if (SpecularFactor > 0.0) {
					SpecularFactor = pow(SpecularFactor, gSpecularPower);
					SpecularColor = vec4(Light.Color * gMatSpecularIntensity * SpecularFactor, 1.0);
				}
			}

					return (AmbientColor + DiffuseColor + SpecularColor);
		}

		vec4 CalcDirectionalLight(vec3 WorldPos, vec3 Normal)
		{
			return CalcLightInternal(gDirectionalLight.Base,
									 gDirectionalLight.Direction,
									 WorldPos,
									 Normal);
		}

		vec4 CalcPointLight(vec3 WorldPos, vec3 Normal)
		{
			vec3 LightDirection = WorldPos - gPointLight.Position;
			float Distance = length(LightDirection);
			LightDirection = normalize(LightDirection);

					vec4 Color = CalcLightInternal(gPointLight.Base, LightDirection, WorldPos, Normal);

					float Attenuation =  gPointLight.Atten.Constant +
								 gPointLight.Atten.Linear * Distance +
								 gPointLight.Atten.Exp * Distance * Distance;

					Attenuation = max(1.0, Attenuation);

					return Color / Attenuation;
		}


		vec2 CalcTexCoord()
		{
			return gl_FragCoord.xy / gScreenSize;
		}

		out vec4 FragColor;

		void main()
		{
			vec2 TexCoord = CalcTexCoord();
			vec3 WorldPos = texture(gPositionMap, TexCoord).xyz;
			vec3 Color = texture(gColorMap, TexCoord).xyz;
			vec3 Normal = texture(gNormalMap, TexCoord).xyz;
			Normal = normalize(Normal);

			FragColor = vec4(Color, 1.0) * CalcDirectionalLight(WorldPos, Normal);
		}
	)", Shader::SourceType::String);

	renderer.setShader(mShaderDirectionalLight);
	renderer.setShaderUniform("gPositionMap", GBuffer2::GBUFFER_TEXTURE_TYPE_POSITION);
	renderer.setShaderUniform("gColorMap", GBuffer2::GBUFFER_TEXTURE_TYPE_DIFFUSE);
	renderer.setShaderUniform("gNormalMap", GBuffer2::GBUFFER_TEXTURE_TYPE_NORMAL);
	renderer.setShaderUniform("gDirectionalLight.Base.Color", mDirLight.Color.x, mDirLight.Color.y, mDirLight.Color.z);
	renderer.setShaderUniform("gDirectionalLight.Base.AmbientIntensity", mDirLight.AmbientIntensity);
	glm::vec3 mDirLightDirection = glm::normalize(mDirLight.Direction);
	renderer.setShaderUniform("gDirectionalLight.Direction", mDirLightDirection.x, mDirLightDirection.y, mDirLightDirection.z);
	renderer.setShaderUniform("gDirectionalLight.Base.DiffuseIntensity", mDirLight.DiffuseIntensity);
	renderer.setShaderUniform("gScreenSize", (float)getViewportWidth(), (float)getViewportHeight());

	mShaderPointLight = renderer.addShader(R"(
		[Vertex]
		#include "core"

		void main()
		{
			vec4 position = ModelViewMatrix * vec4(VertexPosition, 1.0);          
			gl_Position = ProjectionMatrix * position;
		}

		[Fragment]
		#include "core"

		struct BaseLight
		{
			vec3 Color;
			float AmbientIntensity;
			float DiffuseIntensity;
		};

		struct DirectionalLight
		{
			BaseLight Base;
			vec3 Direction;
		};

		struct Attenuation
		{
			float Constant;
			float Linear;
			float Exp;
		};

		struct PointLight
		{
			BaseLight Base;
			vec3 Position;
			Attenuation Atten;
		};

		struct SpotLight
		{
			PointLight Base;
			vec3 Direction;
			float Cutoff;
		};

		uniform sampler2D gPositionMap;
		uniform sampler2D gColorMap;
		uniform sampler2D gNormalMap;
		uniform DirectionalLight gDirectionalLight;
		uniform PointLight gPointLight;
		uniform SpotLight gSpotLight;
		uniform vec3 gEyeWorldPos;
		uniform float gMatSpecularIntensity;
		uniform float gSpecularPower;
		uniform int gLightType;
		uniform vec2 gScreenSize;

		vec4 CalcLightInternal(BaseLight Light,
							   vec3 LightDirection,
							   vec3 WorldPos,
							   vec3 Normal)
		{
			vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0);
			float DiffuseFactor = dot(Normal, -LightDirection);

					vec4 DiffuseColor  = vec4(0, 0, 0, 0);
			vec4 SpecularColor = vec4(0, 0, 0, 0);

					if (DiffuseFactor > 0.0) {
				DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0);

						vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos);
				vec3 LightReflect = normalize(reflect(LightDirection, Normal));
				float SpecularFactor = dot(VertexToEye, LightReflect);        
				if (SpecularFactor > 0.0) {
					SpecularFactor = pow(SpecularFactor, gSpecularPower);
					SpecularColor = vec4(Light.Color * gMatSpecularIntensity * SpecularFactor, 1.0);
				}
			}

					return (AmbientColor + DiffuseColor + SpecularColor);
		}

		vec4 CalcDirectionalLight(vec3 WorldPos, vec3 Normal)
		{
			return CalcLightInternal(gDirectionalLight.Base,
									 gDirectionalLight.Direction,
									 WorldPos,
									 Normal);
		}

		vec4 CalcPointLight(vec3 WorldPos, vec3 Normal)
		{
			vec3 LightDirection = WorldPos - gPointLight.Position;
			float Distance = length(LightDirection);
			LightDirection = normalize(LightDirection);

					vec4 Color = CalcLightInternal(gPointLight.Base, LightDirection, WorldPos, Normal);

					float Attenuation =  gPointLight.Atten.Constant +
								 gPointLight.Atten.Linear * Distance +
								 gPointLight.Atten.Exp * Distance * Distance;

					Attenuation = max(1.0, Attenuation);

					return Color / Attenuation;
		}


		vec2 CalcTexCoord()
		{
			return gl_FragCoord.xy / gScreenSize;
		}

		out vec4 FragColor;

		void main()
		{
			vec2 TexCoord = CalcTexCoord();
			vec3 WorldPos = texture(gPositionMap, TexCoord).xyz;
			vec3 Color = texture(gColorMap, TexCoord).xyz;
			vec3 Normal = texture(gNormalMap, TexCoord).xyz;
			Normal = normalize(Normal);

			FragColor = vec4(Color, 1.0) * CalcPointLight(WorldPos, Normal);
		}
	)", Shader::SourceType::String);

	renderer.setShader(mShaderPointLight);
	renderer.setShaderUniform("gPositionMap", GBuffer2::GBUFFER_TEXTURE_TYPE_POSITION);
	renderer.setShaderUniform("gColorMap", GBuffer2::GBUFFER_TEXTURE_TYPE_DIFFUSE);
	renderer.setShaderUniform("gNormalMap", GBuffer2::GBUFFER_TEXTURE_TYPE_NORMAL);
	renderer.setShaderUniform("gScreenSize", (float)getViewportWidth(), (float)getViewportHeight());
	
	mMesh.loadFromFile("assets/models/leprechaun/leprechaun.fbx");
	//mMesh.makeDrawable(renderer, mShaderGeometry);
	
	mTexD = renderer.addTexture("assets/models/leprechaun/leprechaun_d.png");
	mTexN = renderer.addTexture("assets/models/leprechaun/leprechaun_n.png");
	mTexS = renderer.addTexture("assets/models/leprechaun/leprechaun_s.png");

	camera.setPosition(0, 2, -10);
	camera.setLookAt(0, 4, 0);

	gl::enableDepthRead();
	gl::enableCullFace(gl::CullFaceType::Back);
}

//=========================================================================
Deferred2::~Deferred2()
{
}

//=========================================================================
inline void Deferred2::onInput(const Input &input)
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
inline void Deferred2::onUpdate(double deltaTime)
{
}

//=========================================================================
inline void Deferred2::onDraw()
{
	// --------------------------------------------------------------------
	// Geometry pass
	// --------------------------------------------------------------------
	
	gl::disableAlphaBlending();
	gl::enable3D();

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

	// When we get here the depth buffer is already populated and the stencil pass
	// depends on it, but it does not write to it.
	gl::enable2D();

	// --------------------------------------------------------------------
	// Begin light passes
	// --------------------------------------------------------------------

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	mGBuffer.bindForReading();
	glClear(GL_COLOR_BUFFER_BIT);

	// --------------------------------------------------------------------
	// Point light pass
	// --------------------------------------------------------------------

	renderer.setShader(mShaderPointLight);
	renderer.setShaderUniform("gEyeWorldPos", camera.getPosition());
	
	/*
	Pipeline p;
	p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
	p.SetPerspectiveProj(m_persProjInfo);

	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_pointLight); i++) {
		m_DSPointLightPassTech.SetPointLight(m_pointLight[i]);
		p.WorldPos(m_pointLight[i].Position);
		float BSphereScale = CalcPointLightBSphere(m_pointLight[i]);
		p.Scale(BSphereScale, BSphereScale, BSphereScale);
		m_DSPointLightPassTech.SetWVP(p.GetWVPTrans());
		m_bsphere.Render();
	}
	*/


	//glEnable(GL_BLEND);
	//glBlendEquation(GL_FUNC_ADD);
	//glBlendFunc(GL_ONE, GL_ONE);

	//// ? glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//mGBuffer.bindForReading();
	//glClear(GL_COLOR_BUFFER_BIT);

	// Light pass
	/*glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mGBuffer.bindForReading();

	GLint halfWidth = (GLint)(getViewportWidth() / 2.0f);
	GLint halfHeight = (GLint)(getViewportHeight() / 2.0f);

	mGBuffer.setReadBuffer(GBuffer2::GBUFFER_TEXTURE_TYPE_DIFFUSE);
	glBlitFramebuffer(0, 0, getViewportWidth(), getViewportHeight(), 0, halfHeight, halfWidth, getViewportHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

	mGBuffer.setReadBuffer(GBuffer2::GBUFFER_TEXTURE_TYPE_NORMAL);
	glBlitFramebuffer(0, 0, getViewportWidth(), getViewportHeight(), halfWidth, halfHeight, getViewportWidth(), getViewportHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

	mGBuffer.setReadBuffer(GBuffer2::GBUFFER_TEXTURE_TYPE_POSITION);
	glBlitFramebuffer(0, 0, getViewportWidth(), getViewportHeight(), 0, 0, halfWidth, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	mGBuffer.setReadBuffer(GBuffer2::GBUFFER_TEXTURE_TYPE_TEXCOORD);
	glBlitFramebuffer(0, 0, getViewportWidth(), getViewportHeight(), halfWidth, 0, getViewportWidth(), halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);*/
}

//=========================================================================
inline void Deferred2::onResize(const unsigned int width, const unsigned int height)
{
	camera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
	mGBuffer.resize(width, height);
}
