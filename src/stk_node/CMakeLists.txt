set(BP06_STK_NODE_INTERFACE
        stk_node.cppm
        type_mapper.cppm
        wrapper.cppm
        template_definition.cppm
        template_type_definition.cppm
)

add_library(blueprint_stk_node
        ${BP06_STK_NODE_INTERFACE}
)
target_sources(blueprint_stk_node
    PUBLIC FILE_SET CXX_MODULES FILES ${BP06_STK_NODE_INTERFACE}
)
target_link_libraries(blueprint_stk_node
    PUBLIC Boost::boost blueprint_dyn_node
)

add_executable(type_mapper_test ./test/type_mapper_test.cpp)
target_link_libraries( type_mapper_test
        PRIVATE blueprint_dyn_node blueprint_stk_node GTest::gtest_main
)