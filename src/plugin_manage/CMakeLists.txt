set(BP06_PLUGIN_INTERFACE
        plugin.cppm
        component_package.cppm
)
add_library(blueprint_plugin
    ${BP06_PLUGIN_INTERFACE}
)
target_sources(blueprint_plugin
    PUBLIC FILE_SET CXX_MODULES FILES ${BP06_PLUGIN_INTERFACE}
)

target_link_libraries(blueprint_plugin
        PUBLIC blueprint_dyn_node
        PRIVATE Boost::boost
)