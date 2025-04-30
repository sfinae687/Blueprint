set(BP06_SCHEDULER_INTERFACE
        ./scheduler.cppm
        ./event.cppm
)
add_library(blueprint_scheduler
        ${BP06_SCHEDULER_INTERFACE}
)
target_sources(blueprint_scheduler
        PUBLIC FILE_SET CXX_MODULES FILES ${BP06_SCHEDULER_INTERFACE}
)

add_executable(blueprint_test_event ./test/event_test.cpp)
target_link_libraries(blueprint_test_event PUBLIC blueprint_scheduler GTest::gtest_main)