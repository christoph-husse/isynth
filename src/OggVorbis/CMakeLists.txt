cmake_minimum_required (VERSION 2.8)

project(OggVorbis)

default_source_scan("${SRCDIR}OggVorbis/")

include_directories("${SRCDIR}OggVorbis/")
include_directories("${SRCDIR}Framework/")
include_directories("${SRCDIR}OggVorbis/libvorbis/")
include_directories("${SRCDIR}OggVorbis/libvorbis/include/")
include_directories("${SRCDIR}")

add_library(OggVorbis STATIC ${sources} ${headers})