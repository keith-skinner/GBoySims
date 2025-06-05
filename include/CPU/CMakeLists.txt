cmake_minimum_required(VERSION 3.30)

add_library(CPU INTERFACE)

target_sources(
    CPU
    INTERFACE
        arguments.hpp
        registers.hpp
        memory.hpp
        
        utility/meta.hpp
        utility/bits.hpp
        utility/interval.hpp
)
