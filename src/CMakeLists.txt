add_library(IzInjector STATIC)
add_subdirectory(__test__)

target_sources(IzInjector PRIVATE
	IzInjector.c
	IzInjector.h)

target_include_directories(IzInjector PRIVATE .)

get_target_property(SOURCES IzInjector SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_include_directories(IzInjector INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/IzInjector>)
