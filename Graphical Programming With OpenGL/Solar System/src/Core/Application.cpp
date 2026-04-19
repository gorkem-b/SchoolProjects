#include "Core/Application.h"

#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Scene/CelestialBody.h"
#include "Scene/Scene.h"

#include <glad/gl.h>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <stdexcept>
#include <tuple>
#include <utility>

namespace
{
std::filesystem::path LocateResourceRoot()
{
    auto current = std::filesystem::current_path();

    for (int depth = 0; depth < 5; ++depth)
    {
        const auto candidate = current / "res";
        if (std::filesystem::exists(candidate))
        {
            return candidate;
        }

        if (!current.has_parent_path())
        {
            break;
        }

        current = current.parent_path();
    }

    throw std::runtime_error("Could not locate the res directory.");
}
}

Application::Application(int width, int height, std::string title)
    : m_window(width, height, std::move(title))
    , m_camera(glm::vec3(0.0f, 10.0f, 78.0f))
    , m_width(width)
    , m_height(height)
{
}

Application::~Application()
{
    m_scene.reset();
    m_textures.clear();
    m_skyboxTexture.reset();
    m_skyboxShader.reset();
    m_unlitShader.reset();
    m_litShader.reset();
    m_beltRingMesh.reset();
    m_ringMesh.reset();
    m_skyboxMesh.reset();
    m_sphereMesh.reset();

    m_window.Destroy();

    if (m_glfwInitialized)
    {
        glfwTerminate();
    }
}

void Application::Run()
{
    if (!Initialize())
    {
        throw std::runtime_error("Failed to initialize the application.");
    }

    while (!m_window.ShouldClose())
    {
        const float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - m_lastFrameTime;
        if (m_lastFrameTime == 0.0f)
        {
            deltaTime = 0.0f;
        }

        m_lastFrameTime = currentTime;
        deltaTime = std::min(deltaTime, 0.1f);

        ProcessInput(deltaTime);
        m_scene->Update(deltaTime);
        Render();

        m_window.SwapBuffers();
        m_window.PollEvents();
    }
}

bool Application::Initialize()
{
    if (glfwInit() == GLFW_FALSE)
    {
        throw std::runtime_error("GLFW initialization failed.");
    }

    m_glfwInitialized = true;

    if (!m_window.Initialize())
    {
        throw std::runtime_error("Window creation failed.");
    }

    if (gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)) == 0)
    {
        throw std::runtime_error("Failed to load OpenGL via GLAD.");
    }

    m_resourceRoot = LocateResourceRoot();

    m_window.SetResizeCallback([this](int width, int height) {
        OnResize(width, height);
    });

    m_window.SetMouseMoveCallback([this](double xpos, double ypos) {
        OnMouseMoved(xpos, ypos);
    });

    m_window.CaptureCursor(true);

    glViewport(0, 0, m_width, m_height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glClearColor(0.01f, 0.01f, 0.03f, 1.0f);

    LoadResources();
    BuildScene();
    return true;
}

void Application::LoadResources()
{
    auto loadOptionalTexture = [this](const std::string& relativePath) -> std::shared_ptr<Texture> {
        const auto path = GetResourcePath(relativePath);
        if (!std::filesystem::exists(path))
        {
            return nullptr;
        }

        return Texture::Load2D(path, {255, 255, 255});
    };

    m_sphereMesh = Mesh::CreateSphere(192, 128);
    m_ringMesh = Mesh::CreateRing(1.15f, 2.45f, 512);
    m_beltRingMesh = Mesh::CreateRing(0.96f, 1.08f, 768);
    m_skyboxMesh = Mesh::CreateCube();

    m_litShader = std::make_unique<Shader>(
        GetResourcePath("shaders/basic.vert"),
        GetResourcePath("shaders/light.frag"));

    m_unlitShader = std::make_unique<Shader>(
        GetResourcePath("shaders/basic.vert"),
        GetResourcePath("shaders/unlit.frag"));

    m_skyboxShader = std::make_unique<Shader>(
        GetResourcePath("shaders/skybox.vert"),
        GetResourcePath("shaders/skybox.frag"));

    m_renderer.SetShaders(m_litShader.get(), m_unlitShader.get(), m_skyboxShader.get());

    m_textures["sun"] = Texture::Load2D(GetResourcePath("textures/sun.jpg"), {255, 208, 80});
    m_textures["sun_corona"] = loadOptionalTexture("textures/sun_corona.png");
    m_textures["mercury"] = Texture::Load2D(GetResourcePath("textures/mercury.jpg"), {180, 170, 160});
    m_textures["venus"] = Texture::Load2D(GetResourcePath("textures/venus.jpg"), {216, 164, 96});
    m_textures["earth"] = Texture::Load2D(GetResourcePath("textures/earth.jpg"), {70, 130, 220});
    m_textures["earth_lights"] = loadOptionalTexture("textures/earth_lights.png");
    m_textures["earth_specular"] = loadOptionalTexture("textures/earth_specular.jpg");
    m_textures["earth_clouds"] = loadOptionalTexture("textures/earth_clouds.png");
    m_textures["moon"] = Texture::Load2D(GetResourcePath("textures/moon.jpg"), {204, 204, 204});
    m_textures["mars"] = Texture::Load2D(GetResourcePath("textures/mars.jpg"), {196, 90, 56});
    m_textures["jupiter"] = Texture::Load2D(GetResourcePath("textures/jupiter.jpg"), {214, 166, 121});
    m_textures["saturn"] = Texture::Load2D(GetResourcePath("textures/saturn.jpg"), {210, 186, 120});
    m_textures["uranus"] = Texture::Load2D(GetResourcePath("textures/uranus.jpg"), {165, 214, 222});
    m_textures["neptune"] = Texture::Load2D(GetResourcePath("textures/neptune.jpg"), {66, 103, 214});
    m_textures["pluto"] = Texture::Load2D(GetResourcePath("textures/pluto.jpg"), {170, 156, 142});
    m_textures["asteroid"] = loadOptionalTexture("textures/asteroid.jpg");
    m_textures["dust_ring"] = loadOptionalTexture("textures/dust_ring.png");
    m_textures["icy_moon"] = loadOptionalTexture("textures/icy_moon.jpg");
    m_textures["volcanic_moon"] = loadOptionalTexture("textures/volcanic_moon.jpg");
    m_textures["dark_moon"] = loadOptionalTexture("textures/dark_moon.jpg");
    m_textures["hazy_moon"] = loadOptionalTexture("textures/hazy_moon.jpg");
    m_textures["saturn_ring"] = loadOptionalTexture("textures/saturn_ring.jpg");
    m_textures["saturn_ring_alpha"] = loadOptionalTexture("textures/saturn_ring_alpha.png");

    const std::array<std::filesystem::path, 6> skyboxFaces {
        GetResourcePath("textures/skybox/right.jpg"),
        GetResourcePath("textures/skybox/left.jpg"),
        GetResourcePath("textures/skybox/top.jpg"),
        GetResourcePath("textures/skybox/bottom.jpg"),
        GetResourcePath("textures/skybox/front.jpg"),
        GetResourcePath("textures/skybox/back.jpg")
    };

    const std::array<ColorRGB, 6> skyboxFallbackColors {
        ColorRGB{10, 12, 30},
        ColorRGB{8, 10, 24},
        ColorRGB{12, 14, 32},
        ColorRGB{6, 8, 18},
        ColorRGB{9, 11, 26},
        ColorRGB{7, 9, 22}
    };

    m_skyboxTexture = Texture::LoadCubemap(skyboxFaces, skyboxFallbackColors);
}

void Application::BuildScene()
{
    auto findTexture = [this](const std::string& key) -> std::shared_ptr<Texture> {
        const auto iterator = m_textures.find(key);
        if (iterator == m_textures.end())
        {
            return nullptr;
        }

        return iterator->second;
    };

    auto makeBody = [](CelestialBodyConfig config) {
        return std::make_shared<CelestialBody>(std::move(config));
    };

    auto createMoon = [&](const std::string& name,
                          const std::string& textureKey,
                          float radius,
                          float orbitRadius,
                          float orbitSpeed,
                          float rotationSpeed,
                          float orbitInclination,
                          float axialTilt,
                          float initialOrbitAngle,
                          float ambientStrength,
                          float diffuseStrength,
                          float specularStrength,
                          float shininess,
                          glm::vec3 tint = glm::vec3(1.0f),
                          float atmosphereStrength = 0.0f,
                          glm::vec3 atmosphereColor = glm::vec3(0.0f)) {
        CelestialBodyConfig config;
        config.name = name;
        config.mesh = m_sphereMesh;
        config.texture = findTexture(textureKey);
        if (!config.texture)
        {
            config.texture = m_textures.at("moon");
        }
        config.radius = radius;
        config.orbitRadius = orbitRadius;
        config.orbitSpeed = orbitSpeed;
        config.rotationSpeed = rotationSpeed;
        config.orbitInclination = orbitInclination;
        config.axialTilt = axialTilt;
        config.initialOrbitAngle = initialOrbitAngle;
        config.tint = tint;
        config.ambientStrength = ambientStrength;
        config.diffuseStrength = diffuseStrength;
        config.specularStrength = specularStrength;
        config.shininess = shininess;
        config.atmosphereStrength = atmosphereStrength;
        config.atmosphereColor = atmosphereColor;
        return makeBody(std::move(config));
    };

    auto createMinorBody = [&](const std::string& name,
                               const std::string& textureKey,
                               float radius,
                               float orbitRadius,
                               float orbitSpeed,
                               float rotationSpeed,
                               float orbitInclination,
                               float axialTilt,
                               float initialOrbitAngle,
                               glm::vec3 tint) {
        CelestialBodyConfig config;
        config.name = name;
        config.mesh = m_sphereMesh;
        config.texture = findTexture(textureKey);
        if (!config.texture)
        {
            config.texture = m_textures.at("moon");
        }
        config.radius = radius;
        config.orbitRadius = orbitRadius;
        config.orbitSpeed = orbitSpeed;
        config.rotationSpeed = rotationSpeed;
        config.orbitInclination = orbitInclination;
        config.axialTilt = axialTilt;
        config.initialOrbitAngle = initialOrbitAngle;
        config.tint = tint;
        config.ambientStrength = 0.08f;
        config.diffuseStrength = 0.82f;
        config.specularStrength = 0.04f;
        config.shininess = 8.0f;
        return makeBody(std::move(config));
    };

    CelestialBodyConfig sunConfig;
    sunConfig.name = "Sun";
    sunConfig.mesh = m_sphereMesh;
    sunConfig.texture = m_textures.at("sun");
    sunConfig.radius = 2.6f;
    sunConfig.rotationSpeed = 3.5f;
    sunConfig.axialTilt = 7.25f;
    sunConfig.tint = glm::vec3(1.18f, 1.02f, 0.92f);
    sunConfig.ambientStrength = 0.5f;
    sunConfig.diffuseStrength = 0.12f;
    sunConfig.specularStrength = 0.0f;
    sunConfig.emissionStrength = 2.8f;
    sunConfig.atmosphereStrength = 0.46f;
    sunConfig.atmosphereColor = glm::vec3(1.0f, 0.56f, 0.18f);
    sunConfig.emissive = true;
    auto sun = makeBody(std::move(sunConfig));

    CelestialBodyConfig mercuryConfig;
    mercuryConfig.name = "Mercury";
    mercuryConfig.mesh = m_sphereMesh;
    mercuryConfig.texture = m_textures.at("mercury");
    mercuryConfig.radius = 0.28f;
    mercuryConfig.orbitRadius = 4.4f;
    mercuryConfig.orbitSpeed = 32.0f;
    mercuryConfig.rotationSpeed = 4.0f;
    mercuryConfig.orbitInclination = 2.0f;
    mercuryConfig.initialOrbitAngle = 30.0f;
    mercuryConfig.ambientStrength = 0.09f;
    mercuryConfig.diffuseStrength = 1.05f;
    mercuryConfig.specularStrength = 0.05f;
    mercuryConfig.shininess = 10.0f;
    auto mercury = makeBody(std::move(mercuryConfig));

    CelestialBodyConfig venusConfig;
    venusConfig.name = "Venus";
    venusConfig.mesh = m_sphereMesh;
    venusConfig.texture = m_textures.at("venus");
    venusConfig.radius = 0.45f;
    venusConfig.orbitRadius = 6.4f;
    venusConfig.orbitSpeed = 22.0f;
    venusConfig.rotationSpeed = 1.5f;
    venusConfig.orbitInclination = 3.4f;
    venusConfig.axialTilt = 177.0f;
    venusConfig.initialOrbitAngle = 120.0f;
    venusConfig.ambientStrength = 0.18f;
    venusConfig.diffuseStrength = 0.92f;
    venusConfig.specularStrength = 0.03f;
    venusConfig.shininess = 8.0f;
    venusConfig.atmosphereStrength = 0.12f;
    venusConfig.atmosphereColor = glm::vec3(1.0f, 0.82f, 0.58f);
    auto venus = makeBody(std::move(venusConfig));

    CelestialBodyConfig earthConfig;
    earthConfig.name = "Earth";
    earthConfig.mesh = m_sphereMesh;
    earthConfig.texture = m_textures.at("earth");
    earthConfig.specularMap = findTexture("earth_specular");
    earthConfig.emissionMap = findTexture("earth_lights");
    earthConfig.radius = 0.52f;
    earthConfig.orbitRadius = 8.9f;
    earthConfig.orbitSpeed = 15.0f;
    earthConfig.rotationSpeed = 42.0f;
    earthConfig.axialTilt = 23.4f;
    earthConfig.initialOrbitAngle = 240.0f;
    earthConfig.ambientStrength = 0.05f;
    earthConfig.diffuseStrength = 1.0f;
    earthConfig.specularStrength = 0.55f;
    earthConfig.shininess = 96.0f;
    earthConfig.emissionStrength = 1.35f;
    earthConfig.atmosphereStrength = 0.34f;
    earthConfig.atmosphereColor = glm::vec3(0.35f, 0.62f, 1.0f);
    auto earth = makeBody(std::move(earthConfig));

    CelestialBodyConfig marsConfig;
    marsConfig.name = "Mars";
    marsConfig.mesh = m_sphereMesh;
    marsConfig.texture = m_textures.at("mars");
    marsConfig.radius = 0.38f;
    marsConfig.orbitRadius = 11.8f;
    marsConfig.orbitSpeed = 12.0f;
    marsConfig.rotationSpeed = 38.0f;
    marsConfig.orbitInclination = 1.8f;
    marsConfig.axialTilt = 25.0f;
    marsConfig.initialOrbitAngle = 60.0f;
    marsConfig.ambientStrength = 0.07f;
    marsConfig.diffuseStrength = 0.98f;
    marsConfig.specularStrength = 0.05f;
    marsConfig.shininess = 18.0f;
    auto mars = makeBody(std::move(marsConfig));

    CelestialBodyConfig jupiterConfig;
    jupiterConfig.name = "Jupiter";
    jupiterConfig.mesh = m_sphereMesh;
    jupiterConfig.texture = m_textures.at("jupiter");
    jupiterConfig.radius = 1.05f;
    jupiterConfig.orbitRadius = 16.8f;
    jupiterConfig.orbitSpeed = 8.0f;
    jupiterConfig.rotationSpeed = 80.0f;
    jupiterConfig.orbitInclination = 1.3f;
    jupiterConfig.axialTilt = 3.1f;
    jupiterConfig.initialOrbitAngle = 180.0f;
    jupiterConfig.ambientStrength = 0.12f;
    jupiterConfig.diffuseStrength = 0.94f;
    jupiterConfig.specularStrength = 0.08f;
    jupiterConfig.shininess = 20.0f;
    auto jupiter = makeBody(std::move(jupiterConfig));

    CelestialBodyConfig saturnConfig;
    saturnConfig.name = "Saturn";
    saturnConfig.mesh = m_sphereMesh;
    saturnConfig.texture = m_textures.at("saturn");
    saturnConfig.radius = 0.92f;
    saturnConfig.orbitRadius = 21.6f;
    saturnConfig.orbitSpeed = 6.0f;
    saturnConfig.rotationSpeed = 62.0f;
    saturnConfig.orbitInclination = 2.5f;
    saturnConfig.axialTilt = 26.7f;
    saturnConfig.initialOrbitAngle = 300.0f;
    saturnConfig.ambientStrength = 0.12f;
    saturnConfig.diffuseStrength = 0.92f;
    saturnConfig.specularStrength = 0.07f;
    saturnConfig.shininess = 18.0f;
    auto saturn = makeBody(std::move(saturnConfig));

    CelestialBodyConfig uranusConfig;
    uranusConfig.name = "Uranus";
    uranusConfig.mesh = m_sphereMesh;
    uranusConfig.texture = m_textures.at("uranus");
    uranusConfig.radius = 0.76f;
    uranusConfig.orbitRadius = 26.8f;
    uranusConfig.orbitSpeed = 4.2f;
    uranusConfig.rotationSpeed = 48.0f;
    uranusConfig.orbitInclination = 0.8f;
    uranusConfig.axialTilt = 97.8f;
    uranusConfig.initialOrbitAngle = 28.0f;
    uranusConfig.ambientStrength = 0.11f;
    uranusConfig.diffuseStrength = 0.91f;
    uranusConfig.specularStrength = 0.05f;
    uranusConfig.shininess = 16.0f;
    uranusConfig.atmosphereStrength = 0.1f;
    uranusConfig.atmosphereColor = glm::vec3(0.63f, 0.88f, 0.96f);
    auto uranus = makeBody(std::move(uranusConfig));

    CelestialBodyConfig neptuneConfig;
    neptuneConfig.name = "Neptune";
    neptuneConfig.mesh = m_sphereMesh;
    neptuneConfig.texture = m_textures.at("neptune");
    neptuneConfig.radius = 0.74f;
    neptuneConfig.orbitRadius = 32.8f;
    neptuneConfig.orbitSpeed = 3.2f;
    neptuneConfig.rotationSpeed = 52.0f;
    neptuneConfig.orbitInclination = 1.8f;
    neptuneConfig.axialTilt = 28.3f;
    neptuneConfig.initialOrbitAngle = 210.0f;
    neptuneConfig.ambientStrength = 0.11f;
    neptuneConfig.diffuseStrength = 0.93f;
    neptuneConfig.specularStrength = 0.07f;
    neptuneConfig.shininess = 18.0f;
    neptuneConfig.atmosphereStrength = 0.12f;
    neptuneConfig.atmosphereColor = glm::vec3(0.34f, 0.58f, 1.0f);
    auto neptune = makeBody(std::move(neptuneConfig));

    CelestialBodyConfig plutoConfig;
    plutoConfig.name = "Pluto";
    plutoConfig.mesh = m_sphereMesh;
    plutoConfig.texture = m_textures.at("pluto");
    plutoConfig.radius = 0.24f;
    plutoConfig.orbitRadius = 38.6f;
    plutoConfig.orbitSpeed = 2.0f;
    plutoConfig.rotationSpeed = 8.0f;
    plutoConfig.orbitInclination = 17.2f;
    plutoConfig.axialTilt = 122.5f;
    plutoConfig.initialOrbitAngle = 322.0f;
    plutoConfig.ambientStrength = 0.07f;
    plutoConfig.diffuseStrength = 0.82f;
    plutoConfig.specularStrength = 0.03f;
    plutoConfig.shininess = 10.0f;
    plutoConfig.atmosphereStrength = 0.03f;
    plutoConfig.atmosphereColor = glm::vec3(0.75f, 0.80f, 0.88f);
    auto pluto = makeBody(std::move(plutoConfig));

    earth->AddChild(createMoon("Moon", "moon", 0.16f, 1.3f, 110.0f, 6.0f, 5.1f, 6.7f, 20.0f, 0.08f, 0.95f, 0.04f, 12.0f));

    mars->AddChild(createMoon("Phobos", "dark_moon", 0.07f, 0.78f, 190.0f, 20.0f, 1.0f, 0.5f, 40.0f, 0.08f, 0.86f, 0.03f, 6.0f, glm::vec3(0.88f, 0.84f, 0.8f)));
    mars->AddChild(createMoon("Deimos", "dark_moon", 0.05f, 1.12f, 135.0f, 16.0f, 1.6f, 1.2f, 170.0f, 0.08f, 0.84f, 0.03f, 6.0f, glm::vec3(0.78f, 0.74f, 0.7f)));

    jupiter->AddChild(createMoon("Io", "volcanic_moon", 0.13f, 1.35f, 94.0f, 18.0f, 0.4f, 1.0f, 25.0f, 0.09f, 0.96f, 0.03f, 9.0f, glm::vec3(1.0f, 0.94f, 0.78f)));
    jupiter->AddChild(createMoon("Europa", "icy_moon", 0.12f, 1.7f, 74.0f, 14.0f, 0.5f, 0.1f, 110.0f, 0.08f, 0.92f, 0.03f, 10.0f, glm::vec3(0.88f, 0.96f, 1.0f)));
    jupiter->AddChild(createMoon("Ganymede", "moon", 0.17f, 2.15f, 56.0f, 10.0f, 0.3f, 0.4f, 200.0f, 0.08f, 0.9f, 0.04f, 11.0f, glm::vec3(0.93f, 0.9f, 0.82f)));
    jupiter->AddChild(createMoon("Callisto", "dark_moon", 0.16f, 2.55f, 41.0f, 9.0f, 0.8f, 0.8f, 300.0f, 0.08f, 0.88f, 0.03f, 8.0f, glm::vec3(0.76f, 0.74f, 0.7f)));

    saturn->AddChild(createMoon("Titan", "hazy_moon", 0.18f, 1.72f, 48.0f, 8.0f, 0.6f, 0.2f, 35.0f, 0.09f, 0.93f, 0.03f, 10.0f, glm::vec3(1.0f, 0.86f, 0.6f), 0.04f, glm::vec3(0.95f, 0.7f, 0.28f)));
    saturn->AddChild(createMoon("Enceladus", "icy_moon", 0.08f, 2.22f, 92.0f, 14.0f, 0.1f, 0.3f, 180.0f, 0.08f, 0.9f, 0.03f, 10.0f, glm::vec3(0.92f, 0.98f, 1.0f)));

    uranus->AddChild(createMoon("Titania", "moon", 0.11f, 1.5f, 52.0f, 10.0f, 0.2f, 0.5f, 60.0f, 0.08f, 0.88f, 0.03f, 8.0f, glm::vec3(0.88f, 0.86f, 0.82f)));
    uranus->AddChild(createMoon("Oberon", "dark_moon", 0.1f, 1.92f, 40.0f, 9.0f, 0.5f, 0.4f, 220.0f, 0.08f, 0.86f, 0.03f, 8.0f, glm::vec3(0.74f, 0.72f, 0.72f)));

    neptune->AddChild(createMoon("Triton", "icy_moon", 0.12f, 1.54f, -58.0f, 11.0f, 157.0f, 0.0f, 135.0f, 0.08f, 0.91f, 0.03f, 10.0f, glm::vec3(0.88f, 0.96f, 1.0f)));

    pluto->AddChild(createMoon("Charon", "dark_moon", 0.11f, 0.9f, 86.0f, 10.0f, 0.1f, 0.0f, 15.0f, 0.08f, 0.84f, 0.03f, 8.0f, glm::vec3(0.82f, 0.8f, 0.78f)));

    auto earthCloudTexture = findTexture("earth_clouds");
    if (earthCloudTexture)
    {
        CelestialBodyConfig earthCloudConfig;
        earthCloudConfig.name = "EarthClouds";
        earthCloudConfig.mesh = m_sphereMesh;
        earthCloudConfig.texture = earthCloudTexture;
        earthCloudConfig.opacityMap = earthCloudTexture;
        earthCloudConfig.radius = 1.028f;
        earthCloudConfig.rotationSpeed = 9.0f;
        earthCloudConfig.ambientStrength = 0.05f;
        earthCloudConfig.diffuseStrength = 0.82f;
        earthCloudConfig.specularStrength = 0.04f;
        earthCloudConfig.shininess = 10.0f;
        earthCloudConfig.alpha = 0.78f;
        earthCloudConfig.transparent = true;
        earthCloudConfig.inheritParentScale = true;
        auto earthClouds = makeBody(std::move(earthCloudConfig));
        earth->AddChild(earthClouds);
    }

    auto saturnRingTexture = findTexture("saturn_ring");
    auto saturnRingAlpha = findTexture("saturn_ring_alpha");
    if (saturnRingTexture && saturnRingAlpha)
    {
        CelestialBodyConfig ringConfig;
        ringConfig.name = "SaturnRing";
        ringConfig.mesh = m_ringMesh;
        ringConfig.texture = saturnRingTexture;
        ringConfig.opacityMap = saturnRingAlpha;
        ringConfig.radius = 1.0f;
        ringConfig.ambientStrength = 0.24f;
        ringConfig.diffuseStrength = 0.82f;
        ringConfig.specularStrength = 0.02f;
        ringConfig.shininess = 6.0f;
        ringConfig.alpha = 0.92f;
        ringConfig.transparent = true;
        ringConfig.doubleSided = true;
        ringConfig.inheritParentScale = true;
        auto saturnRing = makeBody(std::move(ringConfig));
        saturn->AddChild(saturnRing);
    }

    const std::array<std::tuple<const char*, float, float, float, float, float, float, float, glm::vec3>, 5> asteroidSpecs {{
        {"CeresLike", 0.09f, 13.7f, 7.5f, 22.0f, 5.0f, 8.0f, 20.0f, glm::vec3(0.95f, 0.9f, 0.82f)},
        {"VestaLike", 0.07f, 14.4f, 6.8f, 32.0f, 7.0f, 14.0f, 110.0f, glm::vec3(0.86f, 0.78f, 0.72f)},
        {"PallasLike", 0.08f, 15.1f, 5.9f, 28.0f, 11.0f, 18.0f, 200.0f, glm::vec3(0.8f, 0.78f, 0.74f)},
        {"HygieaLike", 0.06f, 15.7f, 5.4f, 34.0f, 9.0f, 10.0f, 280.0f, glm::vec3(0.76f, 0.74f, 0.7f)},
        {"BeltRock", 0.05f, 14.9f, 6.3f, 40.0f, 6.0f, 16.0f, 340.0f, glm::vec3(0.92f, 0.84f, 0.76f)}
    }};

    for (const auto& [name, radius, orbitRadius, orbitSpeed, rotationSpeed, orbitInclination, axialTilt, initialOrbitAngle, tint] : asteroidSpecs)
    {
        sun->AddChild(createMinorBody(name, "asteroid", radius, orbitRadius, orbitSpeed, rotationSpeed, orbitInclination, axialTilt, initialOrbitAngle, tint));
    }

    const std::array<std::tuple<const char*, const char*, float, float, float, float, float, float, float, glm::vec3>, 4> outerBeltSpecs {{
        {"KuiperA", "icy_moon", 0.06f, 34.4f, 1.55f, 12.0f, 6.0f, 2.0f, 44.0f, glm::vec3(0.8f, 0.92f, 1.0f)},
        {"KuiperB", "dark_moon", 0.07f, 36.0f, 1.35f, 10.0f, 9.0f, 5.0f, 140.0f, glm::vec3(0.76f, 0.78f, 0.84f)},
        {"KuiperC", "asteroid", 0.05f, 39.8f, 1.1f, 14.0f, 12.0f, 4.0f, 236.0f, glm::vec3(0.82f, 0.86f, 0.92f)},
        {"KuiperD", "icy_moon", 0.06f, 41.4f, 0.95f, 9.0f, 14.0f, 6.0f, 312.0f, glm::vec3(0.84f, 0.92f, 1.0f)}
    }};

    for (const auto& [name, textureKey, radius, orbitRadius, orbitSpeed, rotationSpeed, orbitInclination, axialTilt, initialOrbitAngle, tint] : outerBeltSpecs)
    {
        sun->AddChild(createMinorBody(name, textureKey, radius, orbitRadius, orbitSpeed, rotationSpeed, orbitInclination, axialTilt, initialOrbitAngle, tint));
    }

    auto dustRingTexture = findTexture("dust_ring");
    if (dustRingTexture && m_beltRingMesh)
    {
        CelestialBodyConfig asteroidDustConfig;
        asteroidDustConfig.name = "AsteroidBeltDust";
        asteroidDustConfig.mesh = m_beltRingMesh;
        asteroidDustConfig.texture = dustRingTexture;
        asteroidDustConfig.opacityMap = dustRingTexture;
        asteroidDustConfig.radius = 14.7f;
        asteroidDustConfig.rotationSpeed = 1.2f;
        asteroidDustConfig.axialTilt = 1.4f;
        asteroidDustConfig.tint = glm::vec3(0.96f, 0.78f, 0.58f);
        asteroidDustConfig.ambientStrength = 0.16f;
        asteroidDustConfig.diffuseStrength = 0.3f;
        asteroidDustConfig.specularStrength = 0.0f;
        asteroidDustConfig.shininess = 4.0f;
        asteroidDustConfig.alpha = 0.45f;
        asteroidDustConfig.transparent = true;
        asteroidDustConfig.doubleSided = true;
        sun->AddChild(makeBody(std::move(asteroidDustConfig)));

        CelestialBodyConfig kuiperDustConfig;
        kuiperDustConfig.name = "KuiperDust";
        kuiperDustConfig.mesh = m_beltRingMesh;
        kuiperDustConfig.texture = dustRingTexture;
        kuiperDustConfig.opacityMap = dustRingTexture;
        kuiperDustConfig.radius = 36.2f;
        kuiperDustConfig.rotationSpeed = 0.35f;
        kuiperDustConfig.axialTilt = 9.0f;
        kuiperDustConfig.tint = glm::vec3(0.64f, 0.76f, 0.98f);
        kuiperDustConfig.ambientStrength = 0.12f;
        kuiperDustConfig.diffuseStrength = 0.18f;
        kuiperDustConfig.specularStrength = 0.0f;
        kuiperDustConfig.shininess = 2.0f;
        kuiperDustConfig.alpha = 0.18f;
        kuiperDustConfig.transparent = true;
        kuiperDustConfig.doubleSided = true;
        sun->AddChild(makeBody(std::move(kuiperDustConfig)));
    }

    auto coronaTexture = findTexture("sun_corona");
    if (coronaTexture)
    {
        CelestialBodyConfig coronaConfig;
        coronaConfig.name = "SunCorona";
        coronaConfig.mesh = m_sphereMesh;
        coronaConfig.texture = coronaTexture;
        coronaConfig.opacityMap = coronaTexture;
        coronaConfig.radius = 1.34f;
        coronaConfig.rotationSpeed = 1.8f;
        coronaConfig.tint = glm::vec3(1.2f, 0.8f, 0.34f);
        coronaConfig.ambientStrength = 0.72f;
        coronaConfig.diffuseStrength = 0.02f;
        coronaConfig.specularStrength = 0.0f;
        coronaConfig.emissionStrength = 3.7f;
        coronaConfig.alpha = 0.72f;
        coronaConfig.emissive = true;
        coronaConfig.transparent = true;
        coronaConfig.doubleSided = true;
        coronaConfig.additiveBlending = true;
        coronaConfig.inheritParentScale = true;
        sun->AddChild(makeBody(std::move(coronaConfig)));

        CelestialBodyConfig haloConfig;
        haloConfig.name = "SunHalo";
        haloConfig.mesh = m_sphereMesh;
        haloConfig.texture = coronaTexture;
        haloConfig.opacityMap = coronaTexture;
        haloConfig.radius = 1.82f;
        haloConfig.rotationSpeed = 0.9f;
        haloConfig.tint = glm::vec3(1.1f, 0.7f, 0.28f);
        haloConfig.ambientStrength = 0.48f;
        haloConfig.diffuseStrength = 0.0f;
        haloConfig.specularStrength = 0.0f;
        haloConfig.emissionStrength = 2.1f;
        haloConfig.alpha = 0.2f;
        haloConfig.emissive = true;
        haloConfig.transparent = true;
        haloConfig.doubleSided = true;
        haloConfig.additiveBlending = true;
        haloConfig.inheritParentScale = true;
        sun->AddChild(makeBody(std::move(haloConfig)));
    }

    sun->AddChild(mercury);
    sun->AddChild(venus);
    sun->AddChild(earth);
    sun->AddChild(mars);
    sun->AddChild(jupiter);
    sun->AddChild(saturn);
    sun->AddChild(uranus);
    sun->AddChild(neptune);
    sun->AddChild(pluto);

    m_scene = std::make_unique<Scene>();
    m_scene->SetRoot(sun);
    m_scene->Update(0.0f);
}

void Application::ProcessInput(float deltaTime)
{
    GLFWwindow* handle = m_window.GetNativeHandle();

    if (glfwGetKey(handle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        m_window.SetShouldClose(true);
    }

    if (glfwGetKey(handle, GLFW_KEY_W) == GLFW_PRESS)
    {
        m_camera.ProcessKeyboard(CameraMovement::Forward, deltaTime);
    }

    if (glfwGetKey(handle, GLFW_KEY_S) == GLFW_PRESS)
    {
        m_camera.ProcessKeyboard(CameraMovement::Backward, deltaTime);
    }

    if (glfwGetKey(handle, GLFW_KEY_A) == GLFW_PRESS)
    {
        m_camera.ProcessKeyboard(CameraMovement::Left, deltaTime);
    }

    if (glfwGetKey(handle, GLFW_KEY_D) == GLFW_PRESS)
    {
        m_camera.ProcessKeyboard(CameraMovement::Right, deltaTime);
    }
}

void Application::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderContext context;
    context.view = m_camera.GetViewMatrix();
    context.projection = m_camera.GetProjectionMatrix(static_cast<float>(m_width) / static_cast<float>(std::max(1, m_height)));
    context.cameraPosition = m_camera.GetPosition();
    context.lightPosition = m_scene->GetRootWorldPosition();
    context.time = static_cast<float>(glfwGetTime());

    m_scene->Draw(m_renderer, context);

    const glm::mat4 skyboxView = glm::mat4(glm::mat3(context.view));
    m_renderer.DrawSkybox(*m_skyboxMesh, *m_skyboxTexture, skyboxView, context.projection);
}

void Application::OnMouseMoved(double xpos, double ypos)
{
    if (m_firstMouse)
    {
        m_lastMouseX = xpos;
        m_lastMouseY = ypos;
        m_firstMouse = false;
    }

    const float xOffset = static_cast<float>(xpos - m_lastMouseX);
    const float yOffset = static_cast<float>(m_lastMouseY - ypos);

    m_lastMouseX = xpos;
    m_lastMouseY = ypos;

    m_camera.ProcessMouseMovement(xOffset, yOffset);
}

void Application::OnResize(int width, int height)
{
    m_width = std::max(1, width);
    m_height = std::max(1, height);
    glViewport(0, 0, m_width, m_height);
}

std::filesystem::path Application::GetResourcePath(const std::string& relativePath) const
{
    return m_resourceRoot / relativePath;
}