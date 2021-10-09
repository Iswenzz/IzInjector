cmake_minimum_required(VERSION 3.16.0)
project(IzInjector VERSION 0.1.0 LANGUAGES C)
set(PROJECT_NAME "IzInjector")

# CPack
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR 7Z)
include(CPack)

# Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Build
add_subdirectory(src)
add_executable(IzInjector src/main.c)
target_link_libraries(IzInjector PRIVATE ${CONAN_LIBS} IzInjectorLib)
