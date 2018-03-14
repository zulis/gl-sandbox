#include "App.h"

App::App()
    : BaseApp("Cube")
{
    texture.fromFile("assets/textures/uv.jpg");

    shader.fromString(R"(
		[Vertex]
		#version 430
        layout (location = 0) in vec3 VertexPosition;
        layout (location = 2) in vec2 VertexTexCoord;

        uniform mat4 model;
        uniform mat4 projection;
        uniform mat4 camera;

        out vec2 TexCoord;

		void main()
		{
	        TexCoord = VertexTexCoord;
            gl_Position = projection * camera * model * vec4(VertexPosition, 1.0);
		}

		[Fragment]
		#version 430
        out vec4 FragColor;

        in vec2 TexCoord;

        uniform sampler2D texture1;

        void main()
        {
            vec2 flippedTexCoord = vec2(TexCoord.x, 1.0 - TexCoord.y);
	        FragColor = texture(texture1, TexCoord);
        }
	)");

    std::vector<vec3> vertices =
        {
            // bottom
            vec3(-1.0f, -1.0f, -1.0f),
            vec3(1.0f, -1.0f, -1.0f),
            vec3(-1.0f, -1.0f, 1.0f),
            vec3(1.0f, -1.0f, -1.0f),
            vec3(1.0f, -1.0f, 1.0f),
            vec3(-1.0f, -1.0f, 1.0f),

            // top
            vec3(-1.0f, 1.0f, -1.0f),
            vec3(-1.0f, 1.0f, 1.0f),
            vec3(1.0f, 1.0f, -1.0f),
            vec3(1.0f, 1.0f, -1.0f),
            vec3(-1.0f, 1.0f, 1.0f),
            vec3(1.0f, 1.0f, 1.0f),

            // front
            vec3(-1.0f, -1.0f, 1.0f),
            vec3(1.0f, -1.0f, 1.0f),
            vec3(-1.0f, 1.0f, 1.0f),
            vec3(1.0f, -1.0f, 1.0f),
            vec3(1.0f, 1.0f, 1.0f),
            vec3(-1.0f, 1.0f, 1.0f),

            // back
            vec3(-1.0f, -1.0f, -1.0f),
            vec3(-1.0f, 1.0f, -1.0f),
            vec3(1.0f, -1.0f, -1.0f),
            vec3(1.0f, -1.0f, -1.0f),
            vec3(-1.0f, 1.0f, -1.0f),
            vec3(1.0f, 1.0f, -1.0f),

            // left
            vec3(-1.0f, -1.0f, 1.0f),
            vec3(-1.0f, 1.0f, -1.0f),
            vec3(-1.0f, -1.0f, -1.0f),
            vec3(-1.0f, -1.0f, 1.0f),
            vec3(-1.0f, 1.0f, 1.0f),
            vec3(-1.0f, 1.0f, -1.0f),

            // right
            vec3(1.0f, -1.0f, 1.0f),
            vec3(1.0f, -1.0f, -1.0f),
            vec3(1.0f, 1.0f, -1.0f),
            vec3(1.0f, -1.0f, 1.0f),
            vec3(1.0f, 1.0f, -1.0f),
            vec3(1.0f, 1.0f, 1.0f)
        };

    std::vector<unsigned int> indices = {
        // front
        0, 1, 2,
        2, 3, 0,
        // top
        4, 5, 6,
        6, 7, 4,
        // back
        8, 9, 10,
        10, 11, 8,
        // bottom
        12, 13, 14,
        14, 15, 12,
        // left
        16, 17, 18,
        18, 19, 16,
        // right
        20, 21, 22,
        22, 23, 20
    };

    std::vector<vec2> texCoords =
        {

            // bottom
            vec2(0.0f, 0.0f),
            vec2(1.0f, 0.0f),
            vec2(0.0f, 1.0f),
            vec2(1.0f, 0.0f),
            vec2(1.0f, 1.0f),
            vec2(0.0f, 1.0f),

            // top
            vec2(0.0f, 0.0f),
            vec2(0.0f, 1.0f),
            vec2(1.0f, 0.0f),
            vec2(1.0f, 0.0f),
            vec2(0.0f, 1.0f),
            vec2(1.0f, 1.0f),

            // front
            vec2(1.0f, 0.0f),
            vec2(0.0f, 0.0f),
            vec2(1.0f, 1.0f),
            vec2(0.0f, 0.0f),
            vec2(0.0f, 1.0f),
            vec2(1.0f, 1.0f),

            // back
            vec2(0.0f, 0.0f),
            vec2(0.0f, 1.0f),
            vec2(1.0f, 0.0f),
            vec2(1.0f, 0.0f),
            vec2(0.0f, 1.0f),
            vec2(1.0f, 1.0f),

            // left
            vec2(0.0f, 1.0f),
            vec2(1.0f, 0.0f),
            vec2(0.0f, 0.0f),
            vec2(0.0f, 1.0f),
            vec2(1.0f, 1.0f),
            vec2(1.0f, 0.0f),

            // right
            vec2(1.0f, 1.0f),
            vec2(1.0f, 0.0f),
            vec2(0.0f, 0.0f),
            vec2(1.0f, 1.0f),
            vec2(0.0f, 0.0f),
            vec2(0.0f, 1.0f)
        };

    geometry.setVertices(vertices);
    //geometry.setIndices(indices);
    geometry.setTexCoords(texCoords);
}

void App::update(double deltaTime)
{
    float angle = deltaTime / 1000.0 * 15;  // base 15° per second
    mvp = \
        rotate(mvp, angle * 3.0f, vec3(1, 0, 0)) *  // X axis
        rotate(mvp, angle * 2.0f, vec3(0, 1, 0)) *  // Y axis
        rotate(mat4(1.0f), angle * 4.0f, vec3(0, 0, 1));   // Z axis

    //mvp = glm::mat4(1.0f);
}

void App::draw()
{
    // Set background color as cornflower blue
    glClearColor(0.39f, 0.58f, 0.93f, 1.f);
    // Clear color buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.bind();
    glm::mat4 camera = glm::lookAt(glm::vec3(3, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    shader.setUniform("camera", camera);
    glm::mat4 projection = glm::perspective(glm::radians(50.0f), 1280 / 720.0f, 0.1f, 10.0f);
    shader.setUniform("projection", projection);
    shader.setUniform("model", mvp);

    texture.bind();
    geometry.draw();
}

void App::onResize(int width, int height)
{
    glViewport(0, 0, width, height);
}
