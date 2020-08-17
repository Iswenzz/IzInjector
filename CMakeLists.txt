cmake_minimum_required(VERSION 3.0.0)
project(IzInjector VERSION 0.1.0)

# Select architecture target
set(SELECT_ARCHITECTURE "" CACHE STRING "Select the target architecture.")
set_property(CACHE SELECT_ARCHITECTURE PROPERTY STRINGS x64 x86)

# IzInjector
add_executable(IzInjector
	main.c
	IzInjector.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# 64 bits options (default)
if(SELECT_ARCHITECTURE MATCHES "x64" OR NOT SELECT_ARCHITECTURE)
	message("Selected x64 target.")
	target_link_directories(IzInjector 
		PUBLIC deps/argparse/x64)
endif()

# 32 bits options
if(SELECT_ARCHITECTURE MATCHES "x86")
	message("Selected x86 target.")
	target_link_directories(IzInjector 
		PUBLIC deps/argparse/x86)
	set(CMAKE_C_FLAGS "-m32")
endif()

target_link_libraries(IzInjector libargparse.a)