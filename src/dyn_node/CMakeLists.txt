set(BP06_DYN_NODE_INTERFACE
        dyn_node.cppm
        definition.cppm
        util/utility.cppm
        util/trivial_node.cppm
        util/constant_node.cppm
        util/any_context.cppm
        cpo.cppm
        host_api/host_api.cppm
        byte_archive/byte_archive.cppm
        util/serialization.cppm
)
add_library(blueprint_dyn_node ${BP06_DYN_NODE_INTERFACE}
        util/utility.cpp
        util/trivial_node.cpp
        util/constant_node.cpp
        util/serialize.cpp
)
target_sources(blueprint_dyn_node
    PUBLIC FILE_SET CXX_MODULES FILES ${BP06_DYN_NODE_INTERFACE}
)
target_link_libraries( blueprint_dyn_node
        PUBLIC imnodes msft_proxy
        PRIVATE Boost::boost Boost::log
)