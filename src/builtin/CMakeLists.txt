set(BP06_BUILTIN_NODE_INTERFACE
        builtin_node.cppm
        type/signed_integral.cppm
        type/unsigned_integral.cppm
        node/identity_node.cppm
        node/integral_arithmetic.cppm
        builtin_mapper.cppm
        type/matrix.cppm
        node/matrix_editor.cppm
)
add_library(blueprint_builtin_node
        ${BP06_BUILTIN_NODE_INTERFACE}
        node/identity_node.cpp
        node/matrix_editor.cpp
)
target_sources(blueprint_builtin_node
    PUBLIC FILE_SET CXX_MODULES FILES ${BP06_BUILTIN_NODE_INTERFACE}
)
target_link_libraries(blueprint_builtin_node
    PUBLIC blueprint_dyn_node
    PRIVATE
        Boost::boost
        msft_proxy
        blueprint_plugin
        blueprint_stk_node
        Eigen3::Eigen
        BlueprintOpenCVInterface
)