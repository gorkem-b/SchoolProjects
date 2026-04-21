cmake_minimum_required(VERSION 3.24)

project(FlightSimGL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.8
)
FetchContent_MakeAvailable(glad)
add_subdirectory(${glad_SOURCE_DIR}/cmake ${glad_BINARY_DIR}/glad-cmake)
glad_add_library(glad_gl_core_33 REPRODUCIBLE API gl:core=3.3)

add_library(soil_compat STATIC
    vendor/SOIL/SOIL.cpp
)

target_include_directories(soil_compat PUBLIC vendor)

if(WIN32)
    target_link_libraries(soil_compat PUBLIC windowscodecs ole32)
endif()

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Core/Window.cpp
    src/Core/Input.cpp
    src/Graphics/Hud.cpp
    src/Graphics/Shader.cpp
    src/Graphics/Mesh.cpp
    src/Graphics/Texture.cpp
    src/Graphics/TextureManager.cpp
    src/Graphics/Camera.cpp
    src/Graphics/Skybox.cpp
    src/Graphics/Model.cpp
    src/Simulation/Terrain.cpp
    src/Simulation/Plane.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src vendor)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    GLFW_INCLUDE_NONE
    FLIGHTSIM_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    glad_gl_core_33
    soil_compat
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/res
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)