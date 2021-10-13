add_library(IzInjectorLib
   IzInjector.c IzInjector.h
   Utils.c Utils.h)

add_subdirectory(tests)

target_include_directories(IzInjectorLib PUBLIC .)
