add_library(common INTERFACE)
target_sources(
    common
    INTERFACE
    arguments.hpp
    registers.hpp
    memory.hpp
    utility/meta.hpp
)
target_include_directories(
    common
    INTERFACE
    common
)
target_link_libraries(
    common
    INTERFACE
    cib
)