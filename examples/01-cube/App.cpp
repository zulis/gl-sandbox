#include "App.h"
#include "system/Subsystem.h"
#include "window/Window.h"

App::App()
    : BaseApp("Cube")
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

    std::vector<vec3> vertices = {
        // Front
        vec3(-1.0, -1.0, 1.0),
        vec3(1.0, -1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(-1.0, 1.0, 1.0),
        // Right
        vec3(1.0, -1.0, 1.0),
        vec3(1.0, -1.0, -1.0),
        vec3(1.0, 1.0, -1.0),
        vec3(1.0, 1.0, 1.0),
        // Back
        vec3(-1.0, -1.0, -1.0),
        vec3(-1.0, 1.0, -1.0),
        vec3(1.0, 1.0, -1.0),
        vec3(1.0, -1.0, -1.0),
        // Left
        vec3(-1.0, -1.0, 1.0),
        vec3(-1.0, 1.0, 1.0),
        vec3(-1.0, 1.0, -1.0),
        vec3(-1.0, -1.0, -1.0),
        // Bottom
        vec3(-1.0, -1.0, 1.0),
        vec3(-1.0, -1.0, -1.0),
        vec3(1.0, -1.0, -1.0),
        vec3(1.0, -1.0, 1.0),
        // Top
        vec3(-1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(1.0, 1.0, -1.0),
        vec3(-1.0, 1.0, -1.0)
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    std::vector<vec2> texCoords = {
        // Front
        vec2(0.0f, 0.0f),
        vec2(1.0f, 0.0f),
        vec2(1.0f, 1.0f),
        vec2(0.0f, 1.0f),
        // Right
        vec2(0.0f, 0.0f),
        vec2(1.0f, 0.0f),
        vec2(1.0f, 1.0f),
        vec2(0.0f, 1.0f),
        // Back
        vec2(0.0f, 0.0f),
        vec2(1.0f, 0.0f),
        vec2(1.0f, 1.0f),
        vec2(0.0f, 1.0f),
        // Left
        vec2(0.0f, 0.0f),
        vec2(1.0f, 0.0f),
        vec2(1.0f, 1.0f),
        vec2(0.0f, 1.0f),
        // Bottom
        vec2(0.0f, 0.0f),
        vec2(1.0f, 0.0f),
        vec2(1.0f, 1.0f),
        vec2(0.0f, 1.0f),
        // Top
        vec2(0.0f, 0.0f),
        vec2(1.0f, 0.0f),
        vec2(1.0f, 1.0f),
        vec2(0.0f, 1.0f)
    };

    /*std::vector<vec3> normals = {
        // Front
        vec3(0.0f, 0.0f, 1.0f),
        vec3(0.0f, 0.0f, 1.0f),
        vec3(0.0f, 0.0f, 1.0f),
        vec3(0.0f, 0.0f, 1.0f),
        // Right
        vec3(1.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f),
        // Back
        vec3(0.0f, 0.0f, -1.0f),
        vec3(0.0f, 0.0f, -1.0f),
        vec3(0.0f, 0.0f, -1.0f),
        vec3(0.0f, 0.0f, -1.0f),
        // Left
        vec3(-1.0f, 0.0f, 0.0f),
        vec3(-1.0f, 0.0f, 0.0f),
        vec3(-1.0f, 0.0f, 0.0f),
        vec3(-1.0f, 0.0f, 0.0f),
        // Bottom
        vec3(0.0f, -1.0f, 0.0f),
        vec3(0.0f, -1.0f, 0.0f),
        vec3(0.0f, -1.0f, 0.0f),
        vec3(0.0f, -1.0f, 0.0f),
        // Top
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f)
    };*/

    geometry.setVertices(vertices);
    geometry.setIndices(indices);
    geometry.setTexCoords(texCoords);
    //geometry.setNormals(normals);

    auto& window = subsystem::get<Window>();
    projection = perspective(radians(45.0f), (float)window.getWidth() / (float)window.getHeight(), 0.1f, 1000.0f);
    view = lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    model = mat4(1.0);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void App::update(float deltaTime)
{
    float angle = deltaTime / 1000.0 * 30;
    model = model *
        rotate(mat4(1.0f), angle * 2.0f, vec3(1, 0, 0))  *  // X axis
        rotate(mat4(1.0f), angle * 4.0f, vec3(0, 1, 0)) *  // Y axis
        rotate(mat4(1.0f), angle * 3.0f, vec3(0, 0, 1));  // Z axis

    //model = glm::mat4(1.0f);
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
    geometry.draw();
}

void App::onResize(int width, int height)
{
    projection = perspective(radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
}
