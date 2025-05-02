set(BP06_SCHEDULER_INTERFACE
        ./scheduler.cppm
        ./event.cppm
        poller.cppm
        debug.cppm
        executor.cppm
)
add_library(blueprint_scheduler
        ${BP06_SCHEDULER_INTERFACE}
)
target_sources(blueprint_scheduler
        PUBLIC FILE_SET CXX_MODULES FILES ${BP06_SCHEDULER_INTERFACE}
)
target_link_libraries(blueprint_scheduler
        PRIVATE Boost::boost Boost::log dp::thread-pool
)

# Test

add_executable(blueprint_test_event ./test/event_test.cpp)
target_link_libraries(blueprint_test_event PRIVATE blueprint_scheduler GTest::gtest_main)

add_executable(blueprint_test_poller ./test/poller_test.cpp)
target_link_libraries(blueprint_test_poller
        PRIVATE blueprint_scheduler GTest::gtest_main Boost::log_setup Boost::log
)