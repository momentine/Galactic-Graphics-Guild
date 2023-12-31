cmake_minimum_required(VERSION 3.16)

# Sets project name
project(galactic_graphics_guild LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Sets C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifies required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Xml)

# Allows you to include files from within those directories, without prefixing their filepaths
include_directories(src)

# Specifies .cpp and .h files to be passed to the compiler
add_executable(${PROJECT_NAME}
    src/main.cpp

    src/realtime.cpp
    src/mainwindow.cpp
    src/settings.cpp

    src/shapes/Cube.cpp
    src/shapes/Sphere.cpp
    src/shapes/Cylinder.cpp
    src/shapes/Cone.cpp
    src/utils/scenefilereader.cpp
    src/utils/sceneparser.cpp
    src/camera/camera.cpp

    src/debug.h
    src/mainwindow.h
    src/realtime.h
    src/settings.h

    src/shapes/Cube.h
    src/shapes/Sphere.h
    src/shapes/Cylinder.h
    src/shapes/Cone.h
    src/utils/scenedata.h
    src/utils/scenefilereader.h
    src/utils/sceneparser.h
    src/camera/camera.h
    src/utils/shaderloader.h
    src/utils/aspectratiowidget/aspectratiowidget.hpp
)

# GLM: this creates its library and allows you to `#include "glm/..."`
add_subdirectory(glm)

# GLEW: this creates its library and allows you to `#include "GL/glew.h"`
add_library(StaticGLEW STATIC glew/src/glew.c
    src/camera/camera.h src/camera/camera.cpp)
include_directories(${PROJECT_NAME} PRIVATE glew/include)

# Specifies libraries to be linked (Qt components, glew, etc)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::OpenGL
    Qt::OpenGLWidgets
    Qt::Xml
    StaticGLEW
)

# Specifies other files
qt6_add_resources(${PROJECT_NAME} "Resources"
    PREFIX
        "/"
    FILES

        resources/shaders/default.frag
        resources/shaders/default.vert

        resources/shaders/default_tes.tes
        resources/shaders/default_tes.tcs

        resources/shaders/default_tes.vert
        resources/shaders/texture.vert
        resources/shaders/texture.frag

        resources/shaders/skybox.vert
        resources/shaders/skybox.frag

#        resources/skybox/right.jpg
#        resources/skybox/left.jpg
#        resources/skybox/top.jpg
#        resources/skybox/bottom.jpg
#        resources/skybox/front.jpg
#        resources/skybox/back.jpg

        resources/skybox/skybox_up.png
        resources/skybox/skybox_down.png
        resources/skybox/skybox_right.png
        resources/skybox/skybox_left.png
        resources/skybox/skybox_back.png
        resources/skybox/skybox_front.png

)

target_sources(galactic_graphics_guild_other_files
  PRIVATE
    resources/shaders/skybox.frag
    resources/shaders/skybox.vert
)

# GLEW: this provides support for Windows (including 64-bit)
if (WIN32)
  add_compile_definitions(GLEW_STATIC)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    opengl32
    glu32
  )
endif()

# Set this flag to silence warnings on Windows
if (MSVC OR MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "-Wno-volatile")
endif()
# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()
