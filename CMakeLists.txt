cmake_minimum_required(VERSION 3.16.0)
project(IzInjector VERSION 1.0.1 LANGUAGES C)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# CPack
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR 7Z)
include(CPack)

# Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Tests
include(CTest)
add_executable(IzInjector.Tests src/__test__/main.test.c)
target_link_libraries(IzInjector.Tests PUBLIC ${CONAN_LIBS} IzInjector.Lib)
add_test(NAME Tests COMMAND IzInjector.Tests)

# Build
add_subdirectory(fixture)
add_subdirectory(src)
add_executable(IzInjector src/main.c)
target_link_libraries(IzInjector PRIVATE ${CONAN_LIBS} IzInjector.Lib)

# Pack
install(TARGETS IzInjector DESTINATION bin)
install(TARGETS IzInjector.Lib LIBRARY DESTINATION lib)
install(TARGETS IzInjector.Fixture DESTINATION test)
install(DIRECTORY src/ DESTINATION lib/include/IzInjector 
        FILES_MATCHING PATTERN "*.h" PATTERN "__test__" EXCLUDE)
