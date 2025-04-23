cmake_minimum_required(VERSION 3.28.3)

project(Blueprint LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

include(FetchContent)

# dependency

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
add_subdirectory(lib/glfw)

# Boost
find_package(Boost CONFIG REQUIRED COMPONENTS log log_setup )

# GoogleTest
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# Proxy
add_subdirectory(lib/proxy)

# ImGUI
file(GLOB IMGUI_SOURCE lib/imgui/*.cpp)
list(APPEND IMGUI_SOURCE
        lib/imgui/backends/imgui_impl_glfw.cpp
        lib/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui ${IMGUI_SOURCE})
target_include_directories(imgui
        PUBLIC lib/imgui
        INTERFACE lib/imgui/backends
)

# Blueprint-gui
set(BLUEPRINT_GUI_INTERFACE
        src/gui/gui.cppm
        src/gui/gui_debug.cppm
)
add_library(blueprint_gui ${BLUEPRINT_GUI_INTERFACE}
        src/gui/gui.cpp
)
target_sources(blueprint_gui
    PUBLIC FILE_SET CXX_MODULES FILES ${BLUEPRINT_GUI_INTERFACE}
)
target_link_libraries(blueprint_gui
        PUBLIC imgui OpenGL::GL glfw
        PRIVATE Boost::log
)

add_executable(blueprint src/main.cpp)
target_link_libraries(blueprint
        PRIVATE blueprint_gui Boost::log_setup
)
