add_executable(tries_main main.cpp)

add_library(tries_proxy module_unit.cppm)
target_link_libraries(tries_proxy PUBLIC msft_proxy )
target_sources(tries_proxy
    PUBLIC FILE_SET CXX_MODULES FILES module_unit.cppm
)

target_link_libraries(tries_main PRIVATE tries_proxy)