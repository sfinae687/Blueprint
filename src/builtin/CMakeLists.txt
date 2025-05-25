set(BP06_BUILTIN_NODE_INTERFACE
        builtin_node.cppm
        signed_integral.cppm
        unsigned_integral.cppm
        identity_node.cppm
)
add_library(blueprint_builtin_node ${BP06_BUILTIN_NODE_INTERFACE}
        identity_node.cpp)
target_sources(blueprint_builtin_node
    PUBLIC FILE_SET CXX_MODULES FILES ${BP06_BUILTIN_NODE_INTERFACE}
)
target_link_libraries(blueprint_builtin_node
    PUBLIC blueprint_dyn_node
    PRIVATE Boost::boost msft_proxy blueprint_plugin
)