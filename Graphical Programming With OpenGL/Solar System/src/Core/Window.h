#pragma once

#include <functional>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Window
{
public:
    using ResizeCallback = std::function<void(int, int)>;
    using MouseMoveCallback = std::function<void(double, double)>;

    Window(int width, int height, std::string title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool Initialize();
    void Destroy();

    GLFWwindow* GetNativeHandle() const;
    bool ShouldClose() const;
    void SetShouldClose(bool shouldClose) const;

    void SwapBuffers() const;
    void PollEvents() const;
    void CaptureCursor(bool capture) const;

    void SetResizeCallback(ResizeCallback callback);
    void SetMouseMoveCallback(MouseMoveCallback callback);

    int GetWidth() const;
    int GetHeight() const;

private:
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

    int m_width;
    int m_height;
    std::string m_title;
    GLFWwindow* m_handle{nullptr};
    ResizeCallback m_resizeCallback;
    MouseMoveCallback m_mouseMoveCallback;
};