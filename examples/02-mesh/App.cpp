#include "App.h"

App::App()
    : BaseApp("Mesh")
{
    texture.fromFile("assets/textures/uv.jpg");

    shader.fromString(R"(
		[Vertex]
		#version 430
        layout (location = 0) in vec3 VertexPosition;
	    layout (location = 2) in vec2 VertexTexCoord;

        uniform mat4 projection;
        uniform mat4 view;
        uniform mat4 model;

        out vec2 texCoord;

		void main()
		{
	        texCoord = VertexTexCoord;
            gl_Position = projection * view * model * vec4(VertexPosition, 1.0);
		}

		[Fragment]
		#version 430
        out vec4 FragColor;

        in vec2 texCoord;

        uniform sampler2D colorMap;

        void main()
        {
            vec2 flippedTexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
	        FragColor = texture(colorMap, flippedTexCoord);
        }
	)");

    mesh.fromFile("assets/models/leprechaun/leprechaun.fbx");
    //mesh.fromFile("assets/models/box/box.fbx");

    projection =
        perspective(radians(45.0f), (float) window->getWidth() / (float) window->getHeight(), 0.1f, 1000.0f);
    view = lookAt(glm::vec3(140, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    model = mat4(1.0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void App::update(float deltaTime)
{
    float angle = deltaTime / 1000.0 * 30;
    model = model *
        rotate(mat4(1.0f), angle * 2.0f, vec3(1, 0, 0)) *  // X axis
        rotate(mat4(1.0f), angle * 4.0f, vec3(0, 1, 0)) *  // Y axis
        rotate(mat4(1.0f), angle * 3.0f, vec3(0, 0, 1));   // Z axis
}

void App::draw()
{
    // Set background color as cornflower blue
    glClearColor(0.39f, 0.58f, 0.93f, 1.f);
    // Clear color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.bind();
    shader.setUniform("projection", projection);
    shader.setUniform("view", view);
    shader.setUniform("model", model);

    texture.bind();
    mesh.draw();
}

void App::onResize(int width, int height)
{
    projection = perspective(radians(45.0f), (float) width / (float) height, 0.1f, 1000.0f);
}
