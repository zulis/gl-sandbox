#include "App.h"
#include <imgui.h>
#include <iostream>
#include "system/Subsystem.h"
#include "window/Window.h"
#include "filesystem/FileSystem.h"

using fs = FileSystem;

App::App()
    : BaseApp("Window")
{
    texture.fromFile("assets/textures/uv.jpg");

    shader.fromString(R"(
		[Vertex]
		#version 430
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec2 aTexCoord;

		out vec3 ourColor;
        out vec2 TexCoord;

		void main()
		{
			gl_Position = vec4(aPos, 1.0);
	        ourColor = aColor;
	        TexCoord = vec2(aTexCoord.x, aTexCoord.y);
		}

		[Fragment]
		#version 430
        out vec4 FragColor;

        in vec3 ourColor;
        in vec2 TexCoord;

        uniform sampler2D texture1;

        void main()
        {
	        FragColor = texture(texture1, TexCoord);
        }
	)");

    /*// set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);*/

    std::vector<vec2> vertices =
        {
            vec2(-0.5f, -0.5f),
            vec2(0.5f, -0.5f),
            vec2(-0.5f, 0.5f),
            vec2(0.5f, 0.5f)
        };

    std::vector<unsigned int> indices = { 0, 1, 2, 1, 3, 2 };

    std::vector<vec2> texCoords =
        {

            vec2(0.0f, 1.0f),
            vec2(1.0f, 1.0f),
            vec2(0.0f, 0.0f),
            vec2(1.0f, 0.0f)
        };

    geometry.setVertices(vertices);
    geometry.setIndices(indices);
    geometry.setTexCoords(texCoords);
}

App::~App()
{
    /*glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);*/
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

    shader.bind();
    texture.bind(0);
    geometry.draw();

    //glBindVertexArray(vao);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //ImGui::SetNextWindowSize(ImVec2(400, 300));
    //ImGui::Begin("Hello!");
    //ImGui::End();
}

void App::onResize(int width, int height)
{
    glViewport(0, 0, width, height);
}
