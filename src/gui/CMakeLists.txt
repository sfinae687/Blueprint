set(BP06_GUI_INTERFACE
        gui.cppm
        window.cppm
        gui_debug.cppm
        forward_declaration.cppm
        imgui_begin.cppm
        imnodes_context.cppm
)
add_library(blueprint_gui ${BP06_GUI_INTERFACE}
        window.cpp
        imgui_begin.cpp
        imnodes_context.cpp
)
target_sources(blueprint_gui
        PUBLIC FILE_SET CXX_MODULES FILES ${BP06_GUI_INTERFACE}
)
target_link_libraries(blueprint_gui
        PUBLIC imnodes imgui OpenGL::GL glfw msft_proxy
        PRIVATE imgui_backend Boost::log
)