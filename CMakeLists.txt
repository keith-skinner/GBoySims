cmake_minimum_required(VERSION 3.23.1)

include(FetchContent)
FetchContent_Declare(googletest
    GIT_REPOSITORY  https://github.com/google/googletest
    GIT_TAG         release-1.12.1
)
FetchContent_MakeAvailable(googletest)

project(GB)
find_package(OpenGL)

# add_executable(${PROJECT_NAME})
# target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
# target_compile_options(${PROJECT_NAME} PRIVATE
#     $<$<CONFIG:DEBUG>:-Wall>
#     $<$<CONFIG:DEBUG>:-Wextra>
#     $<$<CONFIG:DEBUG>:-Wpedantic>
#     -Wno-unknown-pragmas
# )
# target_sources(${PROJECT_NAME} PRIVATE
#     src/main.cpp
# )

add_library(utility INTERFACE)
target_include_directories(utility INTERFACE 
    src/utility
)

add_library(LR35902)
target_compile_features(LR35902 PRIVATE cxx_std_20)
set_target_properties(LR35902 PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(LR35902 PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
target_compile_options(LR35902 PRIVATE
    $<$<CONFIG:DEBUG>:-Wall>
    $<$<CONFIG:DEBUG>:-Wextra>
    $<$<CONFIG:DEBUG>:-Wpedantic>
    -Wno-unknown-pragmas
)
target_sources(LR35902 PRIVATE
    src/LR35902/arguments.hpp
    src/LR35902/cpu.cpp
    src/LR35902/cpu.hpp
    src/LR35902/instructions.hpp
    src/LR35902/micro.hpp
    src/LR35902/opcodes.hpp
    src/LR35902/registers.hpp
)
target_link_libraries(LR35902 
    utility
)


add_executable(LR35902_test
    test/LR35902/registers.cpp
    test/LR35902/arguments.cpp
    test/LR35902/micro.cpp
)
target_include_directories(LR35902_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_compile_features(LR35902_test PRIVATE cxx_std_20)
target_compile_options(LR35902_test PRIVATE
    $<$<CONFIG:DEBUG>:-Wall>
    $<$<CONFIG:DEBUG>:-Wextra>
    $<$<CONFIG:DEBUG>:-Wpedantic>
    -Wno-unknown-pragmas
)
target_link_libraries(LR35902_test
    utility
    gtest_main
)
add_test(NAME LR35902_test COMMAND example)