cmake_minimum_required(VERSION 3.21)
project(TestButton VERSION 1.0.0)

# =========================================================
# GEODE SDK
# =========================================================

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "GEODE_SDK environment variable not set!")
endif()

message(STATUS "Using Geode SDK: $ENV{GEODE_SDK}")

# =========================================================
# SOURCE FILES
# =========================================================

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# =========================================================
# INCLUDE GEODE
# =========================================================

include($ENV{GEODE_SDK}/cmake/Geode.cmake)

# =========================================================
# BUILD MOD
# =========================================================

add_library(${PROJECT_NAME} SHARED ${SOURCES})

setup_geode_mod(${PROJECT_NAME})
