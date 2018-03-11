#include "App.h"
#include <imgui.h>
#include <iostream>
#include "filesystem/FileSystem.h"

using fs = FileSystem;

App::App()
    : BaseApp("Window")
{
    const char *data = fs::readFile("assets/textures/uv.jpg");

    tex1.fromFile("assets/textures/uv.jpg");
    tex2.fromFile("assets/textures/uv.jpg");

    sh1.fromString(R"(
		[Vertex]
		#version 430
        layout (location = 0) in vec3 VertexPosition;
		layout (location = 1) in vec3 VertexNormal;
		layout (location = 2) in vec2 VertexTexCoord;
		layout (location = 3) in vec4 VertexTangent;
		layout (location = 4) in vec3 VertexBitangent;

		uniform mat4 ProjectionMatrix;
		uniform mat4 ViewMatrix;
		uniform mat4 ModelMatrix;
		uniform mat4 ModelViewMatrix;
		uniform mat4 MVP;
		uniform mat3 NormalMatrix;

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
		#version 430

		in vec2 TexCoord;
		in vec3 Position;
		in vec3 Normal;

        layout (binding = 0) uniform sampler2D ColorMap;
		layout (binding = 1) uniform sampler2D NormalMap;
		layout (binding = 2) uniform sampler2D HeightMap;
		layout (binding = 3) uniform sampler2D SpecularMap;
		layout (binding = 4) uniform sampler2D EmissiveMap;
		layout (binding = 5) uniform sampler2D OpacityMap;

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
	)");
}

void App::update(double deltaTime)
{
}

void App::draw()
{
    // Set background color as cornflower blue
    glClearColor(0.39f, 0.58f, 0.93f, 1.f);
    // Clear color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sh1.bind();
    tex1.bind(0);
    tex2.bind(1);

    //ImGui::SetNextWindowSize(ImVec2(400, 300));
    //ImGui::Begin("Hello!");
    //ImGui::End();
}

void App::onResize(int width, int height)
{

}
