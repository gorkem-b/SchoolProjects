#pragma once

#include "Core/Window.h"
#include "Graphics/Camera.h"
#include "Graphics/Renderer.h"

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

class Mesh;
class Scene;
class Shader;
class Texture;

class Application
{
public:
    Application(int width, int height, std::string title);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void Run();

private:
    bool Initialize();
    void LoadResources();
    void BuildScene();
    void ProcessInput(float deltaTime);
    void Render();
    void OnMouseMoved(double xpos, double ypos);
    void OnResize(int width, int height);
    std::filesystem::path GetResourcePath(const std::string& relativePath) const;

    Window m_window;
    Renderer m_renderer;
    Camera m_camera;

    std::unique_ptr<Scene> m_scene;

    std::shared_ptr<Mesh> m_sphereMesh;
    std::shared_ptr<Mesh> m_ringMesh;
    std::shared_ptr<Mesh> m_beltRingMesh;
    std::shared_ptr<Mesh> m_skyboxMesh;

    std::unique_ptr<Shader> m_litShader;
    std::unique_ptr<Shader> m_unlitShader;
    std::unique_ptr<Shader> m_skyboxShader;

    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    std::shared_ptr<Texture> m_skyboxTexture;

    int m_width;
    int m_height;
    bool m_firstMouse{true};
    double m_lastMouseX{0.0};
    double m_lastMouseY{0.0};
    float m_lastFrameTime{0.0f};
    std::filesystem::path m_resourceRoot;
    bool m_glfwInitialized{false};
};