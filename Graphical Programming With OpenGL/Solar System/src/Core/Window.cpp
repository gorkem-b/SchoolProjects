#include "Core/Window.h"

#include <utility>

Window::Window(int width, int height, std::string title)
    : m_width(width)
    , m_height(height)
    , m_title(std::move(title))
{
}

Window::~Window()
{
    Destroy();
}

bool Window::Initialize()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_handle = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (m_handle == nullptr)
    {
        return false;
    }

    glfwMakeContextCurrent(m_handle);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(m_handle, this);
    glfwSetFramebufferSizeCallback(m_handle, FramebufferSizeCallback);
    glfwSetCursorPosCallback(m_handle, CursorPositionCallback);
    return true;
}

void Window::Destroy()
{
    if (m_handle != nullptr)
    {
        glfwDestroyWindow(m_handle);
        m_handle = nullptr;
    }
}

GLFWwindow* Window::GetNativeHandle() const
{
    return m_handle;
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_handle) == GLFW_TRUE;
}

void Window::SetShouldClose(bool shouldClose) const
{
    glfwSetWindowShouldClose(m_handle, shouldClose ? GLFW_TRUE : GLFW_FALSE);
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(m_handle);
}

void Window::PollEvents() const
{
    glfwPollEvents();
}

void Window::CaptureCursor(bool capture) const
{
    glfwSetInputMode(m_handle, GLFW_CURSOR, capture ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Window::SetResizeCallback(ResizeCallback callback)
{
    m_resizeCallback = std::move(callback);
}

void Window::SetMouseMoveCallback(MouseMoveCallback callback)
{
    m_mouseMoveCallback = std::move(callback);
}

int Window::GetWidth() const
{
    return m_width;
}

int Window::GetHeight() const
{
    return m_height;
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self == nullptr)
    {
        return;
    }

    self->m_width = width;
    self->m_height = height;

    if (self->m_resizeCallback)
    {
        self->m_resizeCallback(width, height);
    }
}

void Window::CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    auto* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self == nullptr)
    {
        return;
    }

    if (self->m_mouseMoveCallback)
    {
        self->m_mouseMoveCallback(xpos, ypos);
    }
}