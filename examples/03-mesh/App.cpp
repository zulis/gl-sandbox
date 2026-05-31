#include "App.h"
#include "graphics/Primitive.h"

namespace
{
    constexpr float kLightOrbitRadius = 5.6f;
    constexpr float kPrimaryLightHeight = 1.15f;
    constexpr float kSecondaryLightHeight = -0.95f;
    constexpr float kTertiaryLightHeight = 0.25f;
    constexpr float kLightOrbitSpeed = 50.0f;
}

App::App()
    : BaseApp("Mesh")
{
    colorMap.fromFile("assets/models/leprechaun/leprechaun_d.png");
    normalMap.fromFile("assets/models/leprechaun/leprechaun_n.png");
    specularMap.fromFile("assets/models/leprechaun/leprechaun_s.png");
    aoMap.fromFile("assets/models/leprechaun/leprechaun_ao.png");
    emissiveMap.fromFile("assets/models/leprechaun/leprechaun_e.png");

    shader.fromString(R"(
        [Vertex]
        #version 430
        layout (location = 0) in vec3 VertexPosition;
        layout (location = 1) in vec3 VertexNormal;
        layout (location = 2) in vec2 VertexTexCoord;
        layout (location = 3) in vec3 VertexTangent;
        layout (location = 4) in vec3 VertexBitangent;

        uniform mat4 projection;
        uniform mat4 view;
        uniform mat4 model;
        uniform mat3 normalMatrix;

        out vec2 texCoord;
        out vec3 worldPosition;
        out vec3 worldNormal;
        out vec3 worldTangent;
        out vec3 worldBitangent;

        void main()
        {
            vec3 transformedNormal = normalize(normalMatrix * VertexNormal);
            vec3 transformedTangent = normalMatrix * VertexTangent;
            vec3 transformedBitangent = normalMatrix * VertexBitangent;

            if (length(transformedTangent) < 0.0001)
            {
                transformedTangent = abs(transformedNormal.z) < 0.999
                    ? cross(transformedNormal, vec3(0.0, 0.0, 1.0))
                    : cross(transformedNormal, vec3(0.0, 1.0, 0.0));
            }

            transformedTangent = normalize(transformedTangent - dot(transformedTangent, transformedNormal) * transformedNormal);

            if (length(transformedBitangent) < 0.0001)
            {
                transformedBitangent = cross(transformedNormal, transformedTangent);
            }

            transformedBitangent = normalize(transformedBitangent - dot(transformedBitangent, transformedNormal) * transformedNormal);

            texCoord = VertexTexCoord;
            worldNormal = transformedNormal;
            worldTangent = transformedTangent;
            worldBitangent = transformedBitangent;

            vec4 modelPosition = model * vec4(VertexPosition, 1.0);
            worldPosition = modelPosition.xyz;
            gl_Position = projection * view * modelPosition;
        }

        [Fragment]
        #version 430
        out vec4 FragColor;

        struct LightInfo
        {
            bool enabled;
            int type;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec3 position;
            vec3 direction;
            vec3 attenuation;
            float spotCutoff;
            float spotExponent;
            bool usesAttenuation;
            bool usesSpotCone;
        };

        in vec2 texCoord;
        in vec3 worldPosition;
        in vec3 worldNormal;
        in vec3 worldTangent;
        in vec3 worldBitangent;

        uniform sampler2D colorMap;
        uniform sampler2D normalMap;
        uniform sampler2D specularMap;
        uniform sampler2D aoMap;
        uniform sampler2D emissiveMap;
        uniform vec3 viewPos;
        uniform float shininess;
        uniform LightInfo lights[3];

        const int LightTypeDirectional = 0;

        vec3 sampleNormal()
        {
            mat3 tbn = mat3(normalize(worldTangent), normalize(worldBitangent), normalize(worldNormal));
            vec3 mappedNormal = texture(normalMap, texCoord).rgb * 2.0 - 1.0;
            return normalize(tbn * mappedNormal);
        }

        float computeAttenuation(LightInfo light, float distanceToLight)
        {
            if (!light.usesAttenuation)
                return 1.0;

            float falloff = light.attenuation.x + light.attenuation.y * distanceToLight +
                light.attenuation.z * distanceToLight * distanceToLight;
            return 1.0 / max(falloff, 0.0001);
        }

        float computeSpotFactor(LightInfo light, vec3 lightDir)
        {
            if (!light.usesSpotCone)
                return 1.0;

            float theta = dot(normalize(-lightDir), normalize(light.direction));
            float cutoff = cos(radians(light.spotCutoff));

            if (theta <= cutoff)
                return 0.0;

            if (light.spotExponent <= 0.0)
                return 1.0;

            return pow(theta, light.spotExponent);
        }

        vec3 evaluateLight(LightInfo light, vec3 normal, vec3 viewDir, vec3 albedo, vec3 specularColor, float ambientOcclusion)
        {
            if (!light.enabled)
                return vec3(0.0);

            vec3 lightDir;
            float distanceToLight = 0.0;

            if (light.type == LightTypeDirectional)
            {
                lightDir = normalize(-light.direction);
            }
            else
            {
                vec3 lightVector = light.position - worldPosition;
                distanceToLight = length(lightVector);
                lightDir = distanceToLight > 0.0001 ? lightVector / distanceToLight : vec3(0.0, 1.0, 0.0);
            }

            float attenuation = computeAttenuation(light, distanceToLight);
            float spotFactor = computeSpotFactor(light, lightDir);
            float diffuseFactor = max(dot(normal, lightDir), 0.0);
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float specularFactor = diffuseFactor > 0.0
                ? pow(max(dot(normal, halfwayDir), 0.0), shininess)
                : 0.0;

            vec3 ambient = light.ambient * albedo * ambientOcclusion;
            vec3 diffuse = light.diffuse * diffuseFactor * albedo;
            vec3 specular = light.specular * specularFactor * specularColor;

            return ambient + (diffuse + specular) * attenuation * spotFactor;
        }

        void main()
        {
            vec3 albedo = texture(colorMap, texCoord).rgb;
            vec3 specularColor = texture(specularMap, texCoord).rgb;
            float ambientOcclusion = texture(aoMap, texCoord).r;
            vec3 emissive = texture(emissiveMap, texCoord).rgb;
            vec3 normal = sampleNormal();
            vec3 viewDir = normalize(viewPos - worldPosition);

            vec3 lighting = emissive;

            for (int i = 0; i < 3; ++i)
            {
                lighting += evaluateLight(lights[i], normal, viewDir, albedo, specularColor, ambientOcclusion);
            }

            FragColor = vec4(lighting, 1.0);
        }
    )");

    lightMarkerShader.fromString(R"(
        [Vertex]
        #version 430
        layout (location = 0) in vec3 VertexPosition;

        uniform mat4 projection;
        uniform mat4 view;
        uniform mat4 model;

        void main()
        {
            gl_Position = projection * view * model * vec4(VertexPosition, 1.0);
        }

        [Fragment]
        #version 430
        out vec4 FragColor;

        uniform vec3 lightColor;

        void main()
        {
            FragColor = vec4(lightColor, 1.0);
        }
    )");

    lightMarker = Primitive::sphere(0.5f, 24, 12);

    mesh.fromFile("assets/models/leprechaun/leprechaun.fbx");

    model = rotate(model, radians(-90.f), vec3(1.0f, 0.0f, 0.0f));
    model = scale(model, vec3(0.1f));

    camera.setPosition(0, 4, 12);
    camera.setLookAt(lightOrbitCenter);
    camera.setMouseSensitivity(0.008f);

    const float lightPhaseStep = two_pi<float>() / 3.0f;

    lights[0] = Light::point(lightOrbitCenter + vec3(kLightOrbitRadius, kPrimaryLightHeight, 0.0f));
    lights[0].setAmbient(vec3(0.03f, 0.01f, 0.005f));
    lights[0].setDiffuse(vec3(1.35f, 0.68f, 0.24f));
    lights[0].setSpecular(vec3(1.40f, 0.95f, 0.55f));
    lights[0].setAttenuation(1.0f, 0.045f, 0.009f);

    lights[1] = Light::point(lightOrbitCenter + vec3(cos(lightPhaseStep) * kLightOrbitRadius,
                                                      kSecondaryLightHeight,
                                                      sin(lightPhaseStep) * kLightOrbitRadius));
    lights[1].setAmbient(vec3(0.01f, 0.025f, 0.04f));
    lights[1].setDiffuse(vec3(0.18f, 0.74f, 1.35f));
    lights[1].setSpecular(vec3(0.70f, 0.95f, 1.45f));
    lights[1].setAttenuation(1.0f, 0.040f, 0.008f);

    lights[2] = Light::point(lightOrbitCenter + vec3(cos(lightPhaseStep * 2.0f) * kLightOrbitRadius,
                                                      kTertiaryLightHeight,
                                                      sin(lightPhaseStep * 2.0f) * kLightOrbitRadius));
    lights[2].setAmbient(vec3(0.015f, 0.04f, 0.015f));
    lights[2].setDiffuse(vec3(0.44f, 1.20f, 0.38f));
    lights[2].setSpecular(vec3(0.82f, 1.45f, 0.78f));
    lights[2].setAttenuation(1.0f, 0.040f, 0.008f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void App::update(float deltaTime)
{
    lightOrbitTime += deltaTime / 1000.0f;

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

    if (window->isMouseButtonDown(Button::Right))
    {
        window->showMouse(false);
        auto mouseChange = window->getMouseChange();
        camera.rotate(mouseChange.x, mouseChange.y);
    }
    else
    {
        window->showMouse(true);
    }

    const float lightPhaseStep = two_pi<float>() / 3.0f;
    const float primaryAngle = lightOrbitTime * kLightOrbitSpeed;
    const float secondaryAngle = primaryAngle + lightPhaseStep;
    const float tertiaryAngle = primaryAngle + lightPhaseStep * 2.0f;

    lights[0].setPosition(lightOrbitCenter + vec3(cos(primaryAngle) * kLightOrbitRadius,
                                                   kPrimaryLightHeight,
                                                   sin(primaryAngle) * kLightOrbitRadius));
    lights[1].setPosition(lightOrbitCenter + vec3(cos(secondaryAngle) * kLightOrbitRadius,
                                                   kSecondaryLightHeight,
                                                   sin(secondaryAngle) * kLightOrbitRadius));
    lights[2].setPosition(lightOrbitCenter + vec3(cos(tertiaryAngle) * kLightOrbitRadius,
                                                   kTertiaryLightHeight,
                                                   sin(tertiaryAngle) * kLightOrbitRadius));
}

void App::draw()
{
    glClearColor(0.39f, 0.58f, 0.93f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const mat3 normalMatrix = transpose(inverse(mat3(model)));

    shader.bind();
    shader.setUniform("projection", camera.getProjectionMatrix());
    shader.setUniform("view", camera.getViewMatrix());
    shader.setUniform("model", model);
    shader.setUniform("normalMatrix", normalMatrix);
    shader.setUniform("viewPos", camera.getPosition());
    shader.setUniform("shininess", 72.0f);
    shader.setUniform("colorMap", 0);
    shader.setUniform("normalMap", 1);
    shader.setUniform("specularMap", 2);
    shader.setUniform("aoMap", 3);
    shader.setUniform("emissiveMap", 4);
    lights[0].applyToShader(shader, "lights[0]");
    lights[1].applyToShader(shader, "lights[1]");
    lights[2].applyToShader(shader, "lights[2]");

    colorMap.bind(0);
    normalMap.bind(1);
    specularMap.bind(2);
    aoMap.bind(3);
    emissiveMap.bind(4);
    mesh.draw();

    lightMarkerShader.bind();
    lightMarkerShader.setUniform("projection", camera.getProjectionMatrix());
    lightMarkerShader.setUniform("view", camera.getViewMatrix());

    const float primaryMarkerScale = 0.28f + (sin(lightOrbitTime * 5.0f) * 0.03f);
    const float secondaryMarkerScale = 0.24f + (cos(lightOrbitTime * 4.4f) * 0.025f);
    const float tertiaryMarkerScale = 0.26f + (sin(lightOrbitTime * 5.8f) * 0.028f);

    mat4 primaryMarkerModel = translate(mat4(1.0f), lights[0].getPosition());
    primaryMarkerModel = scale(primaryMarkerModel, vec3(primaryMarkerScale));
    lightMarkerShader.setUniform("model", primaryMarkerModel);
    lightMarkerShader.setUniform("lightColor", lights[0].getDiffuse() * 1.15f);
    lightMarker.draw();

    mat4 secondaryMarkerModel = translate(mat4(1.0f), lights[1].getPosition());
    secondaryMarkerModel = scale(secondaryMarkerModel, vec3(secondaryMarkerScale));
    lightMarkerShader.setUniform("model", secondaryMarkerModel);
    lightMarkerShader.setUniform("lightColor", lights[1].getDiffuse() * 1.10f);
    lightMarker.draw();

    mat4 tertiaryMarkerModel = translate(mat4(1.0f), lights[2].getPosition());
    tertiaryMarkerModel = scale(tertiaryMarkerModel, vec3(tertiaryMarkerScale));
    lightMarkerShader.setUniform("model", tertiaryMarkerModel);
    lightMarkerShader.setUniform("lightColor", lights[2].getDiffuse() * 1.12f);
    lightMarker.draw();
}

void App::drawUI()
{
}

void App::onResize(int width, int height)
{
    glViewport(0, 0, width, height);
    camera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
}
