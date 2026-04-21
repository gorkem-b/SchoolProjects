#pragma once

#include <string>

struct GLFWwindow;

class Window
{
public:
    Window() = default;
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool Initialize(const std::string& title, int width, int height);

    bool ShouldClose() const;
    void PollEvents() const;
    void SwapBuffers() const;
    void Close() const;
    void SetTitle(const std::string& title) const;

    GLFWwindow* GetNativeHandle() const;
    float GetAspectRatio() const;

private:
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* m_window = nullptr;
    int m_width = 1280;
    int m_height = 720;
    bool m_glfwInitialized = false;
};