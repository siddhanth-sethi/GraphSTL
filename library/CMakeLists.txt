cmake_minimum_required(VERSION 3.14)

project(
    GraphSTL
    VERSION 1.0.0
    DESCRIPTION "Header-only STL-compatible graph library for C++17"
    HOMEPAGE_URL "https://github.com/siddhanth-sethi/GraphSTL"
    LANGUAGES CXX
)

# Header-only library target
add_library(graphstl INTERFACE)
add_library(graphstl::graphstl ALIAS graphstl)

target_include_directories(graphstl INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(graphstl INTERFACE cxx_std_17)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(GRAPHSTL_IS_TOP_LEVEL TRUE)
else()
    set(GRAPHSTL_IS_TOP_LEVEL FALSE)
endif()

option(GRAPHSTL_BUILD_TESTS "Build GraphSTL test suite" ${GRAPHSTL_IS_TOP_LEVEL})
option(GRAPHSTL_BUILD_BENCHMARKS "Build GraphSTL benchmarks" OFF)
option(GRAPHSTL_BUILD_EXAMPLES "Build GraphSTL examples" OFF)

if(GRAPHSTL_BUILD_TESTS)
    enable_testing()
    add_executable(graphstl_tests tests/test_runner.cpp)
    target_link_libraries(graphstl_tests PRIVATE graphstl)
    target_compile_options(graphstl_tests PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    )
    add_test(NAME graphstl_tests COMMAND graphstl_tests)
endif()

if(GRAPHSTL_BUILD_BENCHMARKS)
    add_executable(graphstl_bench benchmarks/benchmark_dijkstra.cpp)
    target_link_libraries(graphstl_bench PRIVATE graphstl)
    target_compile_options(graphstl_bench PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -O2>
    )
endif()

if(GRAPHSTL_BUILD_EXAMPLES)
    add_executable(graphstl_demo examples/demo.cpp)
    target_link_libraries(graphstl_demo PRIVATE graphstl)
    target_compile_options(graphstl_demo PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>
    )
endif()

# Install rules (for vcpkg and system wide install)
include(GNUInstallDirs)

install(
    TARGETS graphstl
    EXPORT graphstl-targets
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT graphstl-targets
    FILE graphstl-config.cmake
    NAMESPACE graphstl::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/graphstl
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/graphstl-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/graphstl-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/graphstl
)