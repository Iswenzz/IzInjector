add_library(IzInjectorLib
   IzInjector.c
   Utils.c)

add_subdirectory(tests)

target_include_directories(IzInjectorLib PUBLIC .)
