add_library(IzInjectorLib
   IzInjector.c
   Utility.c)

add_subdirectory(tests)

target_include_directories(IzInjectorLib PUBLIC .)
