#include "Core/Input.h"

#include <algorithm>
#include <iterator>

GLFWwindow* Input::s_window = nullptr;
bool Input::s_keys[GLFW_KEY_LAST + 1] = {};
bool Input::s_keysPressed[GLFW_KEY_LAST + 1] = {};
double Input::s_lastMouseX = 0.0;
double Input::s_lastMouseY = 0.0;
float Input::s_mouseDeltaX = 0.0f;
float Input::s_mouseDeltaY = 0.0f;
bool Input::s_firstMouseEvent = true;

void Input::Initialize(GLFWwindow* window)
{
    s_window = window;
    std::fill_n(s_keys, GLFW_KEY_LAST + 1, false);
    std::fill_n(s_keysPressed, GLFW_KEY_LAST + 1, false);
    s_firstMouseEvent = true;
    s_mouseDeltaX = 0.0f;
    s_mouseDeltaY = 0.0f;

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::EndFrame()
{
    s_mouseDeltaX = 0.0f;
    s_mouseDeltaY = 0.0f;
    std::fill_n(s_keysPressed, GLFW_KEY_LAST + 1, false);
}

bool Input::IsKeyDown(int key)
{
    return key >= 0 && key <= GLFW_KEY_LAST && s_keys[key];
}

bool Input::IsKeyPressed(int key)
{
    return key >= 0 && key <= GLFW_KEY_LAST && s_keysPressed[key];
}

float Input::GetMouseDeltaX()
{
    return s_mouseDeltaX;
}

float Input::GetMouseDeltaY()
{
    return s_mouseDeltaY;
}

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    if (key >= 0 && key <= GLFW_KEY_LAST)
    {
        if (action == GLFW_PRESS)
        {
            s_keys[key] = true;
            s_keysPressed[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            s_keys[key] = false;
        }
        else if (action == GLFW_REPEAT)
        {
            s_keys[key] = true;
        }
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void Input::CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    (void)window;

    if (s_firstMouseEvent)
    {
        s_lastMouseX = xpos;
        s_lastMouseY = ypos;
        s_firstMouseEvent = false;
        return;
    }

    s_mouseDeltaX += static_cast<float>(xpos - s_lastMouseX);
    s_mouseDeltaY += static_cast<float>(s_lastMouseY - ypos);

    s_lastMouseX = xpos;
    s_lastMouseY = ypos;
}