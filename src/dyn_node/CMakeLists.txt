set(BP06_DYN_NODE_INTERFACE
        dyn_node.cppm
        builtin_type.cppm
        definition.cppm
        builtin_node.cppm
)
add_library(blueprint_dyn_node ${BP06_DYN_NODE_INTERFACE})
target_sources(blueprint_dyn_node
    PUBLIC FILE_SET CXX_MODULES FILES ${BP06_DYN_NODE_INTERFACE}
)
target_link_libraries( blueprint_dyn_node
        PUBLIC imnodes msft_proxy
        PRIVATE Boost::boost Boost::log
)