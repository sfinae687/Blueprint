set(BP06_CONSTRAINT_INTERFACE
    constraint.cppm
        node_state.cppm
        constraint_information.cppm
)
add_library(blueprint_constraint
    ${BP06_CONSTRAINT_INTERFACE}
)
target_sources(blueprint_constraint
    PUBLIC FILE_SET CXX_MODULES FILES ${BP06_CONSTRAINT_INTERFACE}
)
target_link_libraries(blueprint_constraint
    PUBLIC Boost::boost blueprint_dyn_node blueprint_scheduler blueprint_flow
)