cmake_minimum_required (VERSION 2.8)

project (iSynth)

set(IS_EMSCRIPTEN false CACHE BOOL "Is this build targeting the browser?")

IF(${IS_EMSCRIPTEN})
	add_definitions(-D"IS_EMSCRIPTEN")
ENDIF()

# construct canonical path to source directory
get_filename_component(SRCDIR "${CMAKE_SOURCE_DIR}/src/" REALPATH)
set(SRCDIR "${SRCDIR}/")

# traverse source tree
add_subdirectory(src)