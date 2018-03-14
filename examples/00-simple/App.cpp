#include "App.h"

App::App()
    : BaseApp("Simple")
{
    texture.fromFile("assets/textures/uv.jpg");

    shader.fromString(R"(
		[Vertex]
		#version 430
        layout (location = 0) in vec3 VertexPosition;
        layout (location = 2) in vec2 VertexTexCoord;

		out vec3 ourColor;
        out vec2 TexCoord;

		void main()
		{
			gl_Position = vec4(VertexPosition, 1.0);
	        TexCoord = vec2(VertexTexCoord.x, VertexTexCoord.y);
		}

		[Fragment]
		#version 430
        out vec4 FragColor;

        in vec2 TexCoord;

        uniform sampler2D texture1;

        void main()
        {
	        FragColor = texture(texture1, TexCoord);
        }
	)");

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
    texture.bind();
    geometry.draw();
}

void App::onResize(int width, int height)
{
    glViewport(0, 0, width, height);
}
