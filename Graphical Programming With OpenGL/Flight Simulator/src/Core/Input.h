#pragma once

#include <GLFW/glfw3.h>

class Input
{
public:
    static void Initialize(GLFWwindow* window);
    static void EndFrame();

    static bool IsKeyDown(int key);
    static bool IsKeyPressed(int key);
    static float GetMouseDeltaX();
    static float GetMouseDeltaY();

private:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

    static GLFWwindow* s_window;
    static bool s_keys[GLFW_KEY_LAST + 1];
    static bool s_keysPressed[GLFW_KEY_LAST + 1];
    static double s_lastMouseX;
    static double s_lastMouseY;
    static float s_mouseDeltaX;
    static float s_mouseDeltaY;
    static bool s_firstMouseEvent;
};