cmake_minimum_required(VERSION 3.30)

add_library(utility)

target_sources(
    utility
    PRIVATE
    bits.hpp
    interval.hpp
    meta.hpp
)

target_include_directories(
    utility
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
