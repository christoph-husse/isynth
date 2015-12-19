cmake_minimum_required (VERSION 2.8)

DETECT_ARCH()


file(GLOB_RECURSE SOURCES "${SRCDIR}Engine2D/*")
include_directories("${SRCDIR}")

add_library(Engine2D STATIC ${SOURCES} )

include_directories("${SRCDIR}Framework/")
include_directories("${SRCDIR}../dependencies/")
include_directories("${SRCDIR}")

make_project(Engine2D)
	
IF(NOT ${IS_EMSCRIPTEN})
	include(FindPkgConfig)
	pkg_search_module(SDL2 REQUIRED sdl2)
	include_directories(${SDL2_INCLUDE_DIRS})

	target_link_libraries(Engine2D ${SDL2_LIBRARIES})
ENDIF()
	
