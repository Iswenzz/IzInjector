cmake_minimum_required(VERSION 3.0.0)
project(IzInjector VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(IzInjector
	main.c
	IzInjector.c)

target_link_directories(IzInjector PUBLIC deps/argparse/x86)
target_link_libraries(IzInjector argparse.lib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(CMAKE_C_FLAGS "-Wno-incompatible-pointer-types")
