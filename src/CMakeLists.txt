cmake_minimum_required (VERSION 2.8)

add_subdirectory(Framework)
add_subdirectory(Engine2D)
add_subdirectory(iSynth)
add_subdirectory(MidiParser)
add_subdirectory(OggVorbis)
#add_subdirectory(VlcSweety)
 
# provide a headacheless static version of libSDL that 
# just integrates into the EXE without any further work.
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_subdirectory(libSDL)
ENDIF()
