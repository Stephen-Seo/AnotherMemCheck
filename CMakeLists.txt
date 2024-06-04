cmake_minimum_required(VERSION 3.7)
project(AnotherMemCheck)

set(AnotherMemCheck_VERSION 2.0)
set(AnotherMemCheck_SOVERSION 2)

set(AnotherMemCheck_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/another_memcheck.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/another_memcheck_external.cc"
)

add_compile_options(
    -Wall -Wextra -Wpedantic -Wno-missing-braces
    $<$<COMPILE_LANGUAGE:CXX>:-Weffc++>
    $<$<CONFIG:DEBUG>:-Og>
)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Debug', none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

set(BUILD_SHARED_LIBS On CACHE STRING "Build as shared library." FORCE)

add_library(AnotherMemCheck ${AnotherMemCheck_SOURCES})
set_target_properties(AnotherMemCheck PROPERTIES VERSION ${AnotherMemCheck_VERSION}
                                                 SOVERSION ${AnotherMemCheck_SOVERSION})

target_compile_features(AnotherMemCheck PUBLIC cxx_std_20)
target_link_libraries(AnotherMemCheck PUBLIC pthread)
