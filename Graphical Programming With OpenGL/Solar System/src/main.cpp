#include "Core/Application.h"

#include <exception>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace
{
int RunApplication()
{
    try
    {
        Application application(1280, 720, "Solar System");
        application.Run();
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Application error: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
}

int main()
{
    return RunApplication();
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return RunApplication();
}
#endif