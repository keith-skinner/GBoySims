cmake_minimum_required(VERSION 3.23.1)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY  https://github.com/google/googletest
    GIT_TAG         release-1.12.1
)
FetchContent_MakeAvailable(googletest)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY  https://github.com/gabime/spdlog.git
    GIT_TAG         v1.11.0
)
FetchContent_MakeAvailable(spdlog)


project(GB)
find_package(OpenGL)

add_subdirectory(extern/compile-time-init-build)
add_subdirectory("include")

add_executable(${PROJECT_NAME})
target_compile_features(
    ${PROJECT_NAME}
    PRIVATE
    cxx_std_20
)
target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
    $<$<CONFIG:DEBUG>:-Wall>
    $<$<CONFIG:DEBUG>:-Wextra>
    $<$<CONFIG:DEBUG>:-Wpedantic>
)
target_sources(
    ${PROJECT_NAME}
    PRIVATE
    src/main.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    cib
    LR35902
    spdlog
)

