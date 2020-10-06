
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB CII_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
list(REMOVE_ITEM CII_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/thread.c")
list(REMOVE_ITEM CII_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/memcmp.c")
list(REMOVE_ITEM CII_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/memmove.c")
list(REMOVE_ITEM CII_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/strncmp.c")
list(REMOVE_ITEM CII_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/memchk.c")
list(REMOVE_ITEM CII_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/strerror.c")

if (BUILD_SHARED_LIBS)
    add_definitions(-DCII_BUILD_AS_DLL)
endif (BUILD_SHARED_LIBS)

add_library(cii STATIC ${CII_SOURCES})

target_link_libraries(cii)

set(CII_LINK_WITH cii PARENT_SCOPE)

#-----------------------------------------------------------------------------#
# vim: set ts=8 sts=4 sw=4 et:
