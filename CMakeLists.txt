cmake_minimum_required(VERSION 3.23.1)

include(FetchContent)
FetchContent_Declare(googletest
    GIT_REPOSITORY  https://github.com/google/googletest
    GIT_TAG         release-1.12.1
)
FetchContent_MakeAvailable(googletest)

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
    src/LR35902/registers.hpp
    src/LR35902/micro.hpp
    src/LR35902/instructions.hpp
    src/LR35902/utility/bits.hpp
)

add_executable(LR35902 
    test/LR35902/registers.cpp
    test/LR35902/arguments.cpp
)
target_include_directories(LR35902 PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_compile_features(LR35902 PRIVATE cxx_std_20)
target_compile_options(LR35902 PRIVATE
    $<$<CONFIG:DEBUG>:-Wall>
    $<$<CONFIG:DEBUG>:-Wextra>
    $<$<CONFIG:DEBUG>:-Wpedantic>
    -Wno-unknown-pragmas
)
target_link_libraries(LR35902 gtest_main)
add_test(NAME LR35902 COMMAND example)