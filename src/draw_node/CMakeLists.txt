set(BP06_DRAW_NODE_INTERFACE
        draw_node.cppm
        draw_rule.cppm
        debug.cppm
        builtin/integral.cppm
        builtin/matrix.cppm
        builtin/image.cppm
        builtin/float_point.cppm
)
add_library(blueprint_draw_node
        ${BP06_DRAW_NODE_INTERFACE}
)
target_sources(blueprint_draw_node
    PUBLIC FILE_SET CXX_MODULES FILES ${BP06_DRAW_NODE_INTERFACE}
)
target_link_libraries(blueprint_draw_node
    PUBLIC msft_proxy
    PRIVATE blueprint_dyn_node blueprint_builtin_node imgui Boost::log blueprint_gui BlueprintOpenCVInterface nfd Eigen3::Eigen
)