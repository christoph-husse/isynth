cmake_minimum_required (VERSION 2.8)

default_source_scan("${SRCDIR}MidiParser/")

DETECT_ARCH()

add_library(MidiParser STATIC ${sources} ${headers})

target_link_libraries(MidiParser Engine2D Framework)

include_directories("${SRCDIR}../dependencies/")
include_directories("${SRCDIR}MidiParser/")
include_directories("${SRCDIR}Framework/")
include_directories("${SRCDIR}Engine2D/")
include_directories("${SRCDIR}")

make_project(MidiParser)

USE_BIGOBJ(MidiParser)
	
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	USE_PCH(MidiParser)
endif()
					  