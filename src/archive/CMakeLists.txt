set(BP06_ARCHIVE_INTERFACE
        archive.cppm
)
add_library(blueprint_archive
        ${BP06_ARCHIVE_INTERFACE}
)
target_sources(blueprint_archive
        PUBLIC FILE_SET CXX_MODULES FILES ${BP06_ARCHIVE_INTERFACE}
)
target_link_libraries(blueprint_archive
        PUBLIC cereal::cereal
        PRIVATE Boost::boost
)