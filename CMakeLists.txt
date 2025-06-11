cmake_minimum_required(VERSION 3.28.3)

project(Blueprint LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

include(FetchContent)
#include(cmake/CPM.cmake)

# dependency

set(BUILD_TESTING OFF)

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
add_subdirectory(lib/glfw)

# Cereal
set(SKIP_PERFORMANCE_COMPARISON ON)
add_subdirectory(lib/cereal)
unset(SKIP_PERFORMANCE_COMPARISON)

# Boost
find_package(Boost CONFIG REQUIRED COMPONENTS log log_setup )

# GoogleTest
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

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
set(TP_BUILD_TESTS OFF)
set(TP_BUILD_BENCHMARKS OFF)
set(TP_BUILD_EXAMPLES OFF)
add_subdirectory(lib/thread-pool)

# Image and Matrix
add_subdirectory(lib/eigen)
find_package(OpenCV REQUIRED)
add_library(BlueprintOpenCVInterface INTERFACE)
target_include_directories(BlueprintOpenCVInterface INTERFACE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(BlueprintOpenCVInterface INTERFACE ${OpenCV_LIBS})

# Other support
add_subdirectory(lib/native-file-dialog)

### Blueprint Source ###

set(BUILD_TESTING ON)
enable_testing()

# basic
add_subdirectory(src/archive)

# Blueprint GUI
add_subdirectory(src/gui)

# Blueprint Context
add_subdirectory(src/blueprint)

# Blueprint Dynamic Node Description
add_subdirectory(src/dyn_node)
add_subdirectory(src/stk_node)
add_subdirectory(src/draw_node)
add_subdirectory(src/plugin_manage)
add_subdirectory(src/builtin)

# Blueprint Flow
add_subdirectory(src/flow)

# Blueprint
add_subdirectory(src/application)
add_executable(blueprint_app src/main.cpp)
target_link_libraries(blueprint_app
        PRIVATE blueprint_application_lib Boost::log_setup nfd
)

# Others
add_subdirectory(tries)