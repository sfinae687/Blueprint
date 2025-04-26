set(BP06_NODES_INTERFACE
        nodes.cppm
)
add_library(blueprint_nodes SHARED ${BP06_NODES_INTERFACE}
        nodes.cpp
)
target_sources(blueprint_nodes
    PUBLIC FILE_SET CXX_MODULES FILES ${BP06_NODES_INTERFACE}
)
target_link_libraries(blueprint_nodes PUBLIC imgui)