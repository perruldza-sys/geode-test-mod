cmake_minimum_required(VERSION 3.21)
project(TestButton VERSION 1.0.0)

# =========================================================
# INCLUDE GEODE SDK
# =========================================================

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "GEODE_SDK environment variable not set! "
                        "Install Geode CLI and run: geode sdk install")
endif()

include($ENV{GEODE_SDK}/cmake/Geode.cmake)

# =========================================================
# SOURCE FILES
# =========================================================

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# =========================================================
# BUILD MOD
# =========================================================

geode_plugin_init()

geode_plugin_src(
    ${SOURCES}
)

geode_plugin_target()
