set(BP06_NODE_EDITOR_INTERFACE
    ./node_editor.cppm
)
add_library(node_editor SHARED
    ${BP06_NODE_EDITOR_INTERFACE}
)
target_sources(node_editor
    PUBLIC FILE_SET CXX_MODULES FILES ${BP06_NODE_EDITOR_INTERFACE}
)
target_link_libraries(node_editor
        PUBLIC Boost::boost Boost::log
        PRIVATE imnodes
)