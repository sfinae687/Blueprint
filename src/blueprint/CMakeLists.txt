set(BP06_BLUEPRINT_INTERFACE
    ./blueprint.cppm
    style.cppm
)
add_library(blueprint
    ${BP06_BLUEPRINT_INTERFACE}
    blueprint.cpp
)
target_sources(blueprint
    PUBLIC FILE_SET CXX_MODULES FILES ${BP06_BLUEPRINT_INTERFACE}
)
target_link_libraries(blueprint
    PUBLIC Boost::boost imgui
    PRIVATE blueprint_gui
            blueprint_dyn_node
            blueprint_draw_node
            Boost::log
            blueprint_builtin_node
            blueprint_plugin
            blueprint_flow
            dp::thread-pool
)