#include <array>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Config/Tuning.h"
#include "Core/Input.h"
#include "Core/Window.h"
#include "Graphics/Camera.h"
#include "Graphics/Hud.h"
#include "Graphics/Model.h"
#include "Graphics/Shader.h"
#include "Graphics/Skybox.h"
#include "Graphics/TextureManager.h"
#include "Simulation/Plane.h"
#include "Simulation/Terrain.h"

namespace {
bool IsFiniteVec3(const glm::vec3& value)
{
    return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
}

std::filesystem::path FindFirstExistingPath(std::initializer_list<std::filesystem::path> candidates)
{
    for (const auto& candidate : candidates)
    {
        if (std::filesystem::exists(candidate))
        {
            return candidate;
        }
    }

    return {};
}

std::filesystem::path GetExecutableDirectory()
{
#if defined(_WIN32)
    std::wstring buffer(MAX_PATH, L'\0');
    DWORD length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));

    while (length == buffer.size())
    {
        buffer.resize(buffer.size() * 2);
        length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    }

    if (length == 0)
    {
        return {};
    }

    buffer.resize(length);
    return std::filesystem::path(buffer).parent_path();
#else
    return {};
#endif
}

std::filesystem::path FindResourceRoot()
{
    const std::array<std::filesystem::path, 4> candidates = {
        std::filesystem::path("..") / "res",
        std::filesystem::path("res"),
        std::filesystem::path("build") / "res",
        std::filesystem::path("..") / ".." / "res",
    };

    for (const auto& candidate : candidates)
    {
        if (std::filesystem::exists(candidate))
        {
            return candidate;
        }
    }

    return std::filesystem::path("res");
}

std::string BuildWindowTitle(float fps, const Plane& plane, float altitudeAboveGround,
                             bool paused, bool wireframeEnabled, bool useHeightmapTerrain)
{
    std::ostringstream titleStream;
    titleStream << std::fixed << std::setprecision(1)
                << "Flight Simulator"
                << " | FPS: " << fps
                << " | Speed: " << plane.GetCurrentSpeed() * 3.6f << " km/h"
                << " | Alt: " << altitudeAboveGround
                << " | AoA: " << plane.GetAngleOfAttackDegrees()
                << " | Throttle: " << plane.GetThrottle() * 100.0f << "%"
                << " | Trim: " << plane.GetTrim()
                << " | " << (paused ? "Paused" : "Live")
                << " | " << (wireframeEnabled ? "Wireframe" : "Filled")
                << " | Terrain: " << (useHeightmapTerrain ? "Heightmap" : "Procedural");
    return titleStream.str();
}

void ApplyLightingUniforms(Shader& shader, const glm::mat4& view, const glm::mat4& projection,
                           const Config::LightingSettings& lighting, const glm::vec3& cameraPosition,
                           float fogNear, float fogFar)
{
    shader.SetMat4("view", view);
    shader.SetMat4("projection", projection);
    shader.SetVec3("lightDirection", lighting.direction);
    shader.SetVec3("sunColor", lighting.sunColor);
    shader.SetVec3("skyAmbientColor", lighting.skyAmbientColor);
    shader.SetVec3("groundAmbientColor", lighting.groundAmbientColor);
    shader.SetVec3("cameraPosition", cameraPosition);
    shader.SetVec3("fogColor", lighting.fogColor);
    shader.SetFloat("fogNear", fogNear);
    shader.SetFloat("fogFar", fogFar);
}

HudData BuildHudData(const Plane& plane, float altitudeAboveGround, bool paused)
{
    HudData hudData;
    hudData.altitudeMeters = altitudeAboveGround;
    hudData.speedKmh = plane.GetCurrentSpeed() * 3.6f;
    hudData.indicatedAirspeedKmh = plane.GetIndicatedAirspeedKmh();
    hudData.throttlePercent = plane.GetThrottle() * 100.0f;
    hudData.angleOfAttackDegrees = plane.GetAngleOfAttackDegrees();
    hudData.gForce = plane.GetGForce();
    hudData.pitchDegrees = plane.GetPitchDegrees();
    hudData.bankDegrees = plane.GetBankDegrees();
    hudData.flightPathMarkerX = plane.GetFlightPathMarkerX();
    hudData.flightPathMarkerY = plane.GetFlightPathMarkerY();
    hudData.trimValue = plane.GetTrim();
    hudData.paused = paused;
    return hudData;
}
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    const std::filesystem::path executableDirectory = GetExecutableDirectory();
    if (!executableDirectory.empty())
    {
        std::error_code errorCode;
        std::filesystem::current_path(executableDirectory, errorCode);
    }

    Window window;
    if (!window.Initialize("Flight Simulator", Config::kWindowWidth, Config::kWindowHeight))
    {
        return -1;
    }

    if (gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)) == 0)
    {
        std::cerr << "Failed to initialize GLAD." << std::endl;
        return -1;
    }

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window.GetNativeHandle(), &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    Input::Initialize(window.GetNativeHandle());

    std::cout
        << "Controls:\n"
        << "  W   : Nose down\n"
        << "  S   : Nose up\n"
        << "  A   : Bank / turn left\n"
        << "  D   : Bank / turn right\n"
        << "  P   : Pause / resume simulation\n"
        << "  I   : Toggle wireframe terrain\n"
        << "  R   : Reset plane position\n"
        << "  ESC : Exit\n";
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    const std::filesystem::path resourceRoot = FindResourceRoot();
    const std::filesystem::path shaderRoot = resourceRoot / "shaders";
    const std::filesystem::path textureRoot = resourceRoot / "textures";
    const std::filesystem::path modelRoot = resourceRoot / "models";
    TextureManager textureManager;

    const std::filesystem::path terrainTexturePath = FindFirstExistingPath({
        textureRoot / "terrain_diffuse.png",
        textureRoot / "terrain_diffuse.jpg",
        textureRoot / "terrain_diffuse.ppm",
    });
    const std::filesystem::path planeTexturePath = FindFirstExistingPath({
        textureRoot / "plane_diffuse.png",
        textureRoot / "plane_diffuse.jpg",
        textureRoot / "plane_diffuse.ppm",
    });
    const std::filesystem::path terrainNormalPath = FindFirstExistingPath({
        textureRoot / "terrain_normal.png",
        textureRoot / "terrain_normal.jpg",
    });
    const std::filesystem::path heightmapPath = FindFirstExistingPath({
        textureRoot / "heightmap.png",
        textureRoot / "heightmap.jpg",
        textureRoot / "heightmap.ppm",
    });

    Shader terrainShader;
    Shader modelShader;
    Shader skyboxShader;
    Shader hudShader;

    if (!terrainShader.LoadFromFiles((shaderRoot / "terrain.vert").string(), (shaderRoot / "terrain.frag").string()) ||
        !modelShader.LoadFromFiles((shaderRoot / "model.vert").string(), (shaderRoot / "model.frag").string()) ||
        !skyboxShader.LoadFromFiles((shaderRoot / "skybox.vert").string(), (shaderRoot / "skybox.frag").string()) ||
        !hudShader.LoadFromFiles((shaderRoot / "hud.vert").string(), (shaderRoot / "hud.frag").string()))
    {
        return -1;
    }

    Hud hud;
    hud.Initialize();

    const Texture* terrainTexture = textureManager.Load2D(terrainTexturePath.string(), true);
    const Texture* terrainNormalTexture = !terrainNormalPath.empty() ? textureManager.Load2D(terrainNormalPath.string(), false) : nullptr;
    const Texture* planeTexture = textureManager.Load2D(planeTexturePath.string(), true);

    Terrain terrain;
    const auto& terrainSettings = Config::TerrainGeneration;
    const bool useHeightmapTerrain = !heightmapPath.empty();
    const bool terrainInitialized = useHeightmapTerrain
        ? terrain.InitializeFromHeightmap(heightmapPath.string(), terrainSettings.heightmapHeightScale,
                                          terrainSettings.heightmapSpacing, terrainTexture, terrainNormalTexture)
        : terrain.InitializeProcedural(terrainSettings.proceduralResolution, terrainSettings.proceduralSpacing,
                                       terrainTexture, terrainNormalTexture);

    if (!terrainInitialized)
    {
        std::cerr << "Failed to generate terrain." << std::endl;
        return -1;
    }

    Skybox skybox;
    skybox.Load({
        FindFirstExistingPath({ textureRoot / "skybox" / "right.png", textureRoot / "skybox" / "right.jpg", textureRoot / "skybox" / "right.ppm" }).string(),
        FindFirstExistingPath({ textureRoot / "skybox" / "left.png", textureRoot / "skybox" / "left.jpg", textureRoot / "skybox" / "left.ppm" }).string(),
        FindFirstExistingPath({ textureRoot / "skybox" / "top.png", textureRoot / "skybox" / "top.jpg", textureRoot / "skybox" / "top.ppm" }).string(),
        FindFirstExistingPath({ textureRoot / "skybox" / "bottom.png", textureRoot / "skybox" / "bottom.jpg", textureRoot / "skybox" / "bottom.ppm" }).string(),
        FindFirstExistingPath({ textureRoot / "skybox" / "front.png", textureRoot / "skybox" / "front.jpg", textureRoot / "skybox" / "front.ppm" }).string(),
        FindFirstExistingPath({ textureRoot / "skybox" / "back.png", textureRoot / "skybox" / "back.jpg", textureRoot / "skybox" / "back.ppm" }).string(),
    });

    Model planeModel;
    const std::filesystem::path aircraftModelPath = FindFirstExistingPath({
        modelRoot / "falcon.obj",
        modelRoot / "plane.obj",
    });

    if (!planeModel.Load(aircraftModelPath.string(), planeTexture))
    {
        std::cerr << "Failed to load plane model." << std::endl;
        return -1;
    }

    Plane plane;
    const auto& spawnSettings = Config::Spawn;
    const glm::vec3 spawnPosition(0.0f, terrain.GetHeightAt(0.0f, spawnSettings.zOffset) + spawnSettings.altitudeOffset,
                                  spawnSettings.zOffset);
    const glm::quat spawnOrientation = glm::angleAxis(glm::radians(spawnSettings.pitchDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
    plane.SetSpawnState(spawnPosition, spawnOrientation, spawnSettings.speed, spawnSettings.throttle);
    plane.Reset();
    Camera camera(window.GetAspectRatio());
    camera.UpdateFollowingPlane(plane.GetPosition(), plane.GetVelocity(), plane.GetOrientation(), 0.0f);

    float previousTime = static_cast<float>(glfwGetTime());
    float titleUpdateAccumulator = 0.0f;
    int titleFrameCounter = 0;
    bool paused = false;
    bool wireframeEnabled = false;
    const auto& lighting = Config::Lighting;
    const float fogNear = useHeightmapTerrain ? lighting.fogNearHeightmap : lighting.fogNearProcedural;
    const float fogFar = useHeightmapTerrain ? lighting.fogFarHeightmap : lighting.fogFarProcedural;

    while (!window.ShouldClose())
    {
        const float currentTime = static_cast<float>(glfwGetTime());
        const float deltaTime = currentTime - previousTime;
        previousTime = currentTime;

        window.PollEvents();

        if (Input::IsKeyPressed(GLFW_KEY_P))
        {
            paused = !paused;
        }

        if (Input::IsKeyPressed(GLFW_KEY_I))
        {
            wireframeEnabled = !wireframeEnabled;
            glPolygonMode(GL_FRONT_AND_BACK, wireframeEnabled ? GL_LINE : GL_FILL);
        }

        if (Input::IsKeyPressed(GLFW_KEY_R))
        {
            plane.Reset();
            camera.UpdateFollowingPlane(plane.GetPosition(), plane.GetVelocity(), plane.GetOrientation(), 0.0f);
        }

        if (!paused)
        {
            plane.Update(deltaTime);
        }

        const float halfExtentX = terrain.GetHalfExtentX();
        const float halfExtentZ = terrain.GetHalfExtentZ();
        const float worldWidth = halfExtentX * 2.0f;
        const float worldDepth = halfExtentZ * 2.0f;
        const float wrapMargin = terrainSettings.wrapMargin;

        glm::vec3 wrappedPosition = plane.GetPosition();
        glm::vec3 wrapDelta(0.0f);

        if (wrappedPosition.x > halfExtentX - wrapMargin)
        {
            wrappedPosition.x -= worldWidth;
            wrapDelta.x -= worldWidth;
        }
        else if (wrappedPosition.x < -halfExtentX + wrapMargin)
        {
            wrappedPosition.x += worldWidth;
            wrapDelta.x += worldWidth;
        }

        if (wrappedPosition.z > halfExtentZ - wrapMargin)
        {
            wrappedPosition.z -= worldDepth;
            wrapDelta.z -= worldDepth;
        }
        else if (wrappedPosition.z < -halfExtentZ + wrapMargin)
        {
            wrappedPosition.z += worldDepth;
            wrapDelta.z += worldDepth;
        }

        if (wrapDelta.x != 0.0f || wrapDelta.z != 0.0f)
        {
            plane.SetPosition(wrappedPosition);
            camera.Translate(wrapDelta);
        }

        if (!IsFiniteVec3(plane.GetPosition()) || !IsFiniteVec3(plane.GetVelocity()) || !IsFiniteVec3(camera.GetPosition()))
        {
            plane.Reset();
            camera.UpdateFollowingPlane(plane.GetPosition(), plane.GetVelocity(), plane.GetOrientation(), 0.0f);
        }

        const float terrainHeight = terrain.GetHeightAt(plane.GetPosition().x, plane.GetPosition().z);
        plane.ConstrainAboveGround(terrainHeight + terrainSettings.minimumClearance);
        const float altitudeAboveGround = plane.GetPosition().y - terrainHeight;

        camera.SetAspectRatio(window.GetAspectRatio());
        camera.UpdateFollowingPlane(plane.GetPosition(), plane.GetVelocity(), plane.GetOrientation(), paused ? 0.0f : deltaTime);

        titleUpdateAccumulator += deltaTime;
        ++titleFrameCounter;
        if (titleUpdateAccumulator >= 0.25f)
        {
            const float fps = static_cast<float>(titleFrameCounter) / titleUpdateAccumulator;
            window.SetTitle(BuildWindowTitle(fps, plane, altitudeAboveGround, paused, wireframeEnabled, useHeightmapTerrain));
            titleUpdateAccumulator = 0.0f;
            titleFrameCounter = 0;
        }

        const glm::mat4 view = camera.GetViewMatrix();
        const glm::mat4 projection = camera.GetProjectionMatrix();

        glClearColor(lighting.fogColor.r, lighting.fogColor.g, lighting.fogColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skybox.Draw(skyboxShader, view, projection);

        terrainShader.Use();
        ApplyLightingUniforms(terrainShader, view, projection, lighting, camera.GetPosition(), fogNear, fogFar);
        terrain.Draw(terrainShader, textureManager);

        modelShader.Use();
        ApplyLightingUniforms(modelShader, view, projection, lighting, camera.GetPosition(), fogNear, fogFar);
        modelShader.SetMat4("model", plane.GetModelMatrix());
        planeModel.Draw(modelShader, textureManager);

        hud.Draw(hudShader, framebufferWidth, framebufferHeight, BuildHudData(plane, altitudeAboveGround, paused));

        window.SwapBuffers();
        Input::EndFrame();
    }

    return 0;
}