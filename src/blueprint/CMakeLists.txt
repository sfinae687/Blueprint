set(BP06_BLUEPRINT_INTERFACE
    ./blueprint.cppm
    draw_node.cppm
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
)