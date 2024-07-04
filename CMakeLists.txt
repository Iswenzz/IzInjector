cmake_minimum_required(VERSION 3.27)
project(IzInjector VERSION 1.0.3 LANGUAGES C)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# CMake
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ClangTools)

# CPack
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR 7Z)
include(CPack)

# Tests
if(BUILD_TESTING)
	include(CTest)
	add_executable(IzInjector.Tests)
	find_path(GREATEST_INCLUDE_DIRS "greatest.h")
	target_link_libraries(IzInjector.Tests PRIVATE IzInjector)
	target_include_directories(IzInjector.Tests PRIVATE IzInjector ${GREATEST_INCLUDE_DIRS})
	add_test(NAME Tests COMMAND IzInjector.Tests)
endif()

# Build
add_subdirectory(fixture)
add_subdirectory(src)

if (BUILD_CLI)
	find_package(Argtable3 CONFIG REQUIRED)
	find_package(unofficial-getopt-win32 CONFIG REQUIRED)
	find_path(ARGTABLE3_INCLUDE_DIRS "argtable3.h")
	add_executable(IzInjector.CLI src/main.c)
	set_target_properties(IzInjector.CLI PROPERTIES OUTPUT_NAME IzInjector)
	target_link_libraries(IzInjector.CLI PRIVATE IzInjector argtable3::argtable3)
	target_include_directories(IzInjector.CLI PRIVATE IzInjector ${ARGTABLE3_INCLUDE_DIRS})
	install(TARGETS IzInjector.CLI DESTINATION bin)
endif()

# Install
install(TARGETS IzInjector EXPORT IzInjectorTargets LIBRARY DESTINATION lib)
install(TARGETS IzInjector.Fixture DESTINATION test)
install(DIRECTORY src/ DESTINATION include/IzInjector FILES_MATCHING PATTERN "*.h" PATTERN "__test__" EXCLUDE)

# Package
include(CMakePackageConfigHelpers)

export(EXPORT IzInjectorTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/IzInjectorTargets.cmake"
    NAMESPACE IzInjector::)

configure_package_config_file(cmake/IzInjectorConfig.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/IzInjectorConfig.cmake"
	INSTALL_DESTINATION lib/cmake/IzInjector)

install(EXPORT IzInjectorTargets
    FILE IzInjectorTargets.cmake
    NAMESPACE IzInjector::
    DESTINATION lib/cmake/IzInjector)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/IzInjectorConfig.cmake"
    DESTINATION lib/cmake/IzInjector)
