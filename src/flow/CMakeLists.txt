set(BP06_FLOW_INTERFACE
    flow.cppm
        link_manage.cppm
        node_instance_manage.cppm
        debug.cppm
        NOID.cppm
        link_events.cppm
)

set(BP06_CONSTRAINT_INTERFACE
        constraint.cppm
        node_state.cppm
        constraint_flow.cppm
        constraint_debug.cppm
)

add_library(blueprint_flow
        ${BP06_FLOW_INTERFACE}
        node_instance_manage.cpp
        link_manage.cpp
        ${BP06_CONSTRAINT_INTERFACE}
        constraint_flow.cpp
)

target_sources(blueprint_flow
    PUBLIC FILE_SET CXX_MODULES FILES ${BP06_FLOW_INTERFACE} ${BP06_CONSTRAINT_INTERFACE}
)
target_link_libraries(blueprint_flow
    PUBLIC blueprint_dyn_node blueprint_scheduler Boost::log Boost::boost
)

add_executable(blueprint_flow_NOID_test ./test/flow_NOID_test.cpp)
target_link_libraries(blueprint_flow_NOID_test
    PRIVATE GTest::gtest_main blueprint_flow
)
add_executable(blueprint_flow_node_test test/node_instance_manager_test.cpp)
target_link_libraries(blueprint_flow_node_test
    PRIVATE GTest::gtest_main blueprint_flow
)
add_executable(blueprint_flow_flow_test test/flow_manager_test.cpp)
target_link_libraries(blueprint_flow_flow_test
    PRIVATE GTest::gtest_main blueprint_flow
)
add_executable(blueprint_flow_link_event_test test/flow_link_manager_event_test.cpp)
target_link_libraries(blueprint_flow_link_event_test
    PRIVATE GTest::gtest_main blueprint_flow
)

gtest_discover_tests(blueprint_flow_NOID_test)
gtest_discover_tests(blueprint_flow_node_test)
gtest_discover_tests(blueprint_flow_flow_test)
gtest_discover_tests(blueprint_flow_link_event_test)