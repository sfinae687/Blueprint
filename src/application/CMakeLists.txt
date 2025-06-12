set(BP06_APP_INTERFACE
        application.cppm
)
add_library(blueprint_application_lib
    ${BP06_APP_INTERFACE}
    application.cpp
)
target_sources(blueprint_application_lib
    PUBLIC FILE_SET CXX_MODULES FILES ${BP06_APP_INTERFACE}
)
target_link_libraries(blueprint_application_lib
        PUBLIC blueprint
        PRIVATE blueprint_gui Boost::log Boost::boost nfd
)