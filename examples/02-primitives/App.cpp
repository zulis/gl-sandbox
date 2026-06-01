#include "App.h"

App::App()
    : BaseApp("Primitives")
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

    geometry1 = Primitive::box();
    geometry2 = Primitive::cone();
    geometry3 = Primitive::sphere();
    geometry4 = Primitive::capsule();

    projection =
        perspective(radians(45.0f), (float)window->getWidth() / (float)window->getHeight(), 0.1f, 1000.0f);
    view = lookAt(glm::vec3(2.5, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    model = mat4(1.0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void App::update(float deltaTime)
{
    float angle = deltaTime / 1000.0 * 30;
    model = model *
            rotate(mat4(1.0f), angle * 2.0f, vec3(1, 0, 0)) * // X axis
            rotate(mat4(1.0f), angle * 4.0f, vec3(0, 1, 0)) * // Y axis
            rotate(mat4(1.0f), angle * 3.0f, vec3(0, 0, 1));  // Z axis
}

void App::draw()
{
    glClearColor(0.39f, 0.58f, 0.93f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const int windowWidth = window->getWidth();
    const int windowHeight = window->getHeight();
    const int halfWidth = windowWidth / 2;
    const int halfHeight = windowHeight / 2;

    shader.bind();
    shader.setUniform("projection", projection);
    shader.setUniform("view", view);
    shader.setUniform("model", model);

    texture.bind();

    glViewport(0, halfHeight, halfWidth, windowHeight - halfHeight);
    geometry1.draw();

    glViewport(halfWidth, halfHeight, windowWidth - halfWidth, windowHeight - halfHeight);
    geometry2.draw();

    glViewport(0, 0, halfWidth, halfHeight);
    geometry3.draw();

    glViewport(halfWidth, 0, windowWidth - halfWidth, halfHeight);
    geometry4.draw();

    glViewport(0, 0, windowWidth, windowHeight);
}

void App::drawUI()
{
}

void App::onResize(int width, int height)
{
    glViewport(0, 0, width, height);
    projection = perspective(radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
}
