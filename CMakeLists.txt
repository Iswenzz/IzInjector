cmake_minimum_required(VERSION 3.0.0)
project(IzInjector VERSION 0.1.0)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# Select architecture target
set(SELECT_ARCHITECTURE "x64" CACHE STRING "Select the target architecture.")
set_property(CACHE SELECT_ARCHITECTURE PROPERTY STRINGS x64 x86)

# IzInjector
add_executable(IzInjector
	main.c
	IzInjector.c)

# IzInjector link directories
target_link_directories(IzInjector PUBLIC
	$<$<STREQUAL:"${SELECT_ARCHITECTURE}","x64">:${CMAKE_CURRENT_SOURCE_DIR}/deps/argparse/x64>
	$<$<STREQUAL:"${SELECT_ARCHITECTURE}","x86">:${CMAKE_CURRENT_SOURCE_DIR}/deps/argparse/x86>)

# IzInjector link libraries
target_link_libraries(IzInjector
	$<$<LINK_LANG_AND_ID:C,MSVC>:libargparse.lib>
	$<$<LINK_LANG_AND_ID:C,GNU,Clang>:libargparse.a>)