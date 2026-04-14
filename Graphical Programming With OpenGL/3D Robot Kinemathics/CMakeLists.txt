cmake_minimum_required(VERSION 3.16)

project(RobotKinematics LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/App.cpp
    src/Camera.cpp
    src/Robot.cpp
    src/Input.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        src
)

if(TARGET OpenGL::GL)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
elseif(TARGET OpenGL::OpenGL)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::OpenGL)
elseif(OPENGL_gl_LIBRARY)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_gl_LIBRARY})
endif()

if(TARGET OpenGL::GLU)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GLU)
elseif(OPENGL_glu_LIBRARY)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_glu_LIBRARY})
endif()

if(TARGET GLUT::GLUT)
    target_link_libraries(${PROJECT_NAME} PRIVATE GLUT::GLUT)
else()
    if(GLUT_INCLUDE_DIRS)
        target_include_directories(${PROJECT_NAME} PRIVATE ${GLUT_INCLUDE_DIRS})
    endif()

    if(GLUT_LIBRARIES)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${GLUT_LIBRARIES})
    endif()
endif()

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX)
endif()