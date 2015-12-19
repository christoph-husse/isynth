cmake_minimum_required (VERSION 2.8)

default_source_scan("${SRCDIR}iSynth/")

DETECT_ARCH()


add_executable(iSynth WIN32 ${sources} ${headers})

include_directories("${SRCDIR}Framework/")
include_directories("${SRCDIR}Engine2D/")
include_directories("${SRCDIR}iSynthLib/")
include_directories("${SRCDIR}iSynth/")
include_directories("${SRCDIR}OggVorbis/")
include_directories("${SRCDIR}MidiParser/")
include_directories("${SRCDIR}")

make_project(iSynth)
	
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	USE_PCH(iSynth)
	
	target_link_libraries(iSynth Imm32.lib Version.lib Winmm.lib)
endif()
					  
target_link_libraries(iSynth Engine2D Framework OggVorbis MidiParser)

IF(${IS_EMSCRIPTEN})
	get_target_property(tmpPath "iSynth" RUNTIME_OUTPUT_DIRECTORY)
	postbuild_copy_file(iSynth "${tmpPath}/iSynth" "${CMAKE_SOURCE_DIR}/bin/${ARCH}/iSynth.bc")
ELSE()
	postbuild_copy_runtime(iSynth "${CMAKE_SOURCE_DIR}/bin/${ARCH}/")
ENDIF()


add_custom_command(TARGET iSynth POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       "${SRCDIR}/Assets" "${CMAKE_SOURCE_DIR}/bin/${ARCH}/Assets/")
					   