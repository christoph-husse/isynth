cmake_minimum_required (VERSION 2.8)

project(Framework)

file(GLOB_RECURSE SOURCES "${SRCDIR}Framework/*")
include_directories("${SRCDIR}")

add_library(Framework STATIC ${SOURCES} )
