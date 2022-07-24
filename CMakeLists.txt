cmake_minimum_required(VERSION 3.23.1)

project(GB)
find_package(OpenGL)

add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:DEBUG>:-Wall>
    $<$<CONFIG:DEBUG>:-Wextra>
    $<$<CONFIG:DEBUG>:-Wpedantic>
    -Wno-unknown-pragmas
)

target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
)