cmake_minimum_required(VERSION 3.21)
project(TestButton VERSION 1.0.0)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "GEODE_SDK not set!")
endif()

include($ENV{GEODE_SDK}/cmake/Geode.cmake)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

setup_geode_mod(${PROJECT_NAME})
