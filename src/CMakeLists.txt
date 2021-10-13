add_subdirectory(tests)

add_library(IzInjectorLib
   IzInjector.c IzInjector.h
   Utils.c Utils.h)

set_target_properties(IzInjectorLib PROPERTIES PUBLIC_HEADER src/IzInjector.h)
set_target_properties(IzInjectorLib PROPERTIES OUTPUT_NAME IzInjector)
target_include_directories(IzInjectorLib PUBLIC .)
