cmake_minimum_required(VERSION 3.21)
project(TestButton VERSION 1.0.0)

# Grab all source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please clone https://github.com/geode-sdk/geode "
                         "and set the GEODE_SDK environment variable to point to it "
                         "(or install it via the Geode CLI: `geode sdk install`).")
endif()

message(STATUS "Using Geode SDK: $ENV{GEODE_SDK}")
add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})
