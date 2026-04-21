#include "Core/Window.h"

#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

Window::~Window()
{
    if (m_window != nullptr)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    if (m_glfwInitialized)
    {
        glfwTerminate();
        m_glfwInitialized = false;
    }
}

bool Window::Initialize(const std::string& title, int width, int height)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return false;
    }

    m_glfwInitialized = true;
    m_width = width;
    m_height = height;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = monitor != nullptr ? glfwGetVideoMode(monitor) : nullptr;

    int windowWidth = m_width;
    int windowHeight = m_height;
    if (videoMode != nullptr)
    {
        windowWidth = videoMode->width;
        windowHeight = videoMode->height;
        m_width = windowWidth;
        m_height = windowHeight;
    }

    m_window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), monitor, nullptr);
    if (m_window == nullptr)
    {
        m_window = glfwCreateWindow(m_width, m_height, title.c_str(), nullptr, nullptr);
        if (m_window == nullptr)
        {
            std::cerr << "Failed to create GLFW window." << std::endl;
            glfwTerminate();
            m_glfwInitialized = false;
            return false;
        }
    }

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
    glfwSwapInterval(1);

    return true;
}

bool Window::ShouldClose() const
{
    return m_window == nullptr || glfwWindowShouldClose(m_window) != 0;
}

void Window::PollEvents() const
{
    glfwPollEvents();
}

void Window::SwapBuffers() const
{
    if (m_window != nullptr)
    {
        glfwSwapBuffers(m_window);
    }
}

void Window::Close() const
{
    if (m_window != nullptr)
    {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }
}

void Window::SetTitle(const std::string& title) const
{
    if (m_window != nullptr)
    {
        glfwSetWindowTitle(m_window, title.c_str());
    }
}

GLFWwindow* Window::GetNativeHandle() const
{
    return m_window;
}

float Window::GetAspectRatio() const
{
    return m_height == 0 ? 1.0f : static_cast<float>(m_width) / static_cast<float>(m_height);
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (instance != nullptr)
    {
        instance->m_width = width;
        instance->m_height = height;
    }
}