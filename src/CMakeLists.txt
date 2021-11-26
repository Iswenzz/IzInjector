add_subdirectory(__test__)

add_library(IzInjectorLib)
set_target_properties(IzInjectorLib PROPERTIES OUTPUT_NAME IzInjector)

target_sources(IzInjectorLib PRIVATE
   IzInjector.c IzInjector.h)

target_include_directories(IzInjectorLib PUBLIC .)

get_target_property(SOURCES IzInjectorLib SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
