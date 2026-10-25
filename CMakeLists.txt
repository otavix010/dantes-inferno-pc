# dantesinferno - ReXGlue Recompiled Project
#
# SDK boilerplate lives in generated/rexglue.cmake.

cmake_minimum_required(VERSION 3.25)
project(dantesinferno LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(generated/rexglue.cmake)

# Sources
set(DANTESINFERNO_SOURCES
    src/main.cpp
)

if(WIN32)
    add_executable(dantesinferno WIN32 ${DANTESINFERNO_SOURCES})
else()
    add_executable(dantesinferno ${DANTESINFERNO_SOURCES})
endif()

rexglue_setup_target(dantesinferno GPU_PLUGINS xenos)

# the ports' common part: linker map, -march=native on the player's PC, the FidelityFX runtime (port.cmake)
include(port.cmake)
port_setup(dantesinferno)
