#include "App.h"

App::App()
    : BaseApp("Mesh")
{
    colorMap.fromFile("assets/models/leprechaun/leprechaun_d.png");

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
	        FragColor = texture(colorMap, texCoord);
        }
	)");

    mesh.fromFile("assets/models/leprechaun/leprechaun.fbx");

    model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(180.f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.1f));

    camera.setPosition(0, 2, -10);
    camera.setLookAt(0, 4, 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void App::update(float deltaTime)
{
    if (window->isKeyShiftDown())
        camera.setStrafeSpeed(14.0f * deltaTime);
    else
        camera.setStrafeSpeed(12.0f * deltaTime);

    if (window->isKeyDown(Key::W))
        camera.move(Camera::FORWARD);
    else if (window->isKeyDown(Key::S))
        camera.move(Camera::BACKWARD);

    if (window->isKeyDown(Key::A))
        camera.move(Camera::LEFT);
    else if (window->isKeyDown(Key::D))
        camera.move(Camera::RIGHT);

    if (window->isKeyDown(Key::E))
        camera.move(Camera::UP);
    else if (window->isKeyDown(Key::Q))
        camera.move(Camera::DOWN);

    if (window->getMouseWheelChange() != 0)
        camera.move(window->getMouseWheelChange() > 0 ? Camera::FORWARD : Camera::BACKWARD);

    if (window->isMouseButtonDown(Button::Right)) {
        window->showMouse(false);
        auto mouseChange = window->getMouseChange();
        //camera.rotate(mouseChange.x * .5f, mouseChange.y * .5f);
        camera.rotate(mouseChange.x * deltaTime * 15.0f, mouseChange.y * deltaTime * 15.0f);
    }
    else {
        window->showMouse(true);
    }
}

void App::draw()
{
    glClearColor(0.39f, 0.58f, 0.93f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.bind();
    shader.setUniform("projection", camera.getProjectionMatrix());
    shader.setUniform("view", camera.getViewMatrix());
    shader.setUniform("model", model);

    colorMap.bind();
    mesh.draw();
}

void App::onResize(int width, int height)
{
    glViewport(0, 0, width, height);
    camera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
}
