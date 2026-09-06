cmake_minimum_required(VERSION 3.21)
project(TestButton VERSION 1.0.0)

# =========================================================
# SOURCE FILES
# =========================================================

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# =========================================================
# GEODE SDK
# =========================================================

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "GEODE_SDK not set!")
endif()

include($ENV{GEODE_SDK}/cmake/Geode.cmake)

# =========================================================
# BUILD
# =========================================================

geode_plugin_init()

geode_plugin_src(
    ${SOURCES}
)

geode_plugin_target()
