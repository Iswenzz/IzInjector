add_subdirectory(__test__)

add_library(IzInjector.Lib)
set_target_properties(IzInjector.Lib PROPERTIES OUTPUT_NAME IzInjector)

target_sources(IzInjector.Lib PRIVATE
   IzInjector.c IzInjector.h)

target_include_directories(IzInjector.Lib PUBLIC .)

get_target_property(SOURCES IzInjector.Lib SOURCES)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
