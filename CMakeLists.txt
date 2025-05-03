cmake_minimum_required(VERSION 3.28.3)

project(Blueprint LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

include(FetchContent)
include(cmake/CPM.cmake)

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

add_library(imgui ${IMGUI_SOURCE})
target_include_directories(imgui
        PUBLIC lib/imgui
)

set(IMGUI_BACKEND_SOURCE
        lib/imgui/backends/imgui_impl_glfw.cpp
        lib/imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui_backend ${IMGUI_BACKEND_SOURCE})
target_include_directories(imgui_backend PUBLIC lib/imgui/backends)
target_link_libraries(imgui_backend PUBLIC imgui)

# ImNodes
set(IMNODES_IMGUI_TARGET_NAME imgui)
add_subdirectory(lib/imnodes)

# ThreadPool
CPMAddPackage(
        NAME thread-pool
        GITHUB_REPOSITORY DeveloperPaul123/thread-pool
        GIT_TAG 0.7.0 # change this to latest commit or release tag
        OPTIONS
        "TP_BUILD_TESTS OFF"
        "TP_BUILD_BENCHMARKS OFF"
        "TP_BUILD_EXAMPLES OFF"
)

# Blueprint Source

enable_testing()

# Blueprint GUI
add_subdirectory(src/gui)

# Blueprint Context
add_subdirectory(src/blueprint)

# Blueprint Event and Scheduler
add_subdirectory(src/scheduler)

# Blueprint Dynamic Node Description
add_subdirectory(src/dyn_node)
add_subdirectory(src/draw_node)

# Blueprint
add_executable(blueprint_app src/main.cpp)
target_link_libraries(blueprint_app
        PRIVATE blueprint Boost::log_setup
)
