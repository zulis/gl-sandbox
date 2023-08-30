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

    float v[] = {
        // Front
        -1.0, -1.0, 1.0,
        1.0, -1.0, 1.0,
        1.0, 1.0, 1.0,
        -1.0, 1.0, 1.0,
        // Right
        1.0, -1.0, 1.0,
        1.0, -1.0, -1.0,
        1.0, 1.0, -1.0,
        1.0, 1.0, 1.0,
        // Back
        -1.0, -1.0, -1.0,
        -1.0, 1.0, -1.0,
        1.0, 1.0, -1.0,
        1.0, -1.0, -1.0,
        // Left
        -1.0, -1.0, 1.0,
        -1.0, 1.0, 1.0,
        -1.0, 1.0, -1.0,
        -1.0, -1.0, -1.0,
        // Bottom
        -1.0, -1.0, 1.0,
        -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,
        1.0, -1.0, 1.0,
        // Top
        -1.0, 1.0, 1.0,
        1.0, 1.0, 1.0,
        1.0, 1.0, -1.0,
        -1.0, 1.0, -1.0};

    unsigned int vLength = sizeof(v) / sizeof(*v);
    std::vector<glm::vec3> vertices(vLength);
    std::memcpy(&vertices[0].x, &v[0], vLength * sizeof(float));

    unsigned int i[] = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23};

    unsigned int iLength = sizeof(i) / sizeof(*i);
    std::vector<unsigned int> indices(iLength);
    std::memcpy(&indices[0], &i[0], iLength * sizeof(unsigned int));

    float tx[] = {
        // Front
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // Right
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // Back
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // Left
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // Bottom
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // Top
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f};

    unsigned int txLength = sizeof(tx) / sizeof(*tx);
    std::vector<glm::vec2> texCoords(txLength);
    std::memcpy(&texCoords[0].x, &tx[0], txLength * sizeof(float));

    float n[] = {
        // Front
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        // Right
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        // Back
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        // Left
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        // Bottom
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        // Top
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f};

    unsigned int nLength = sizeof(n) / sizeof(*n);
    std::vector<glm::vec3> normals(nLength);
    std::memcpy(&normals[0].x, &n[0], nLength * sizeof(float));

    geometry.setVertices(vertices);
    geometry.setIndices(indices);
    geometry.setTexCoords(texCoords);
    geometry.setNormals(normals); // Not used in this example

    projection =
        perspective(radians(45.0f), (float)window->getWidth() / (float)window->getHeight(), 0.1f, 1000.0f);
    view = lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
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

    shader.bind();
    shader.setUniform("projection", projection);
    shader.setUniform("view", view);
    shader.setUniform("model", model);

    texture.bind();
    geometry.draw();
}

void App::drawUI()
{
    static bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);
}

void App::onResize(int width, int height)
{
    glViewport(0, 0, width, height);
    projection = perspective(radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
}
