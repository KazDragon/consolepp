if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
    project(CONSOLEPP VERSION 0.0.1)
else()
    project(CONSOLEPP)
endif()

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 OLD) # Do not allow hidden visibility for static libs
endif()

option(CONSOLEPP_WITH_TESTS "Build with tests included" False)
option(CONSOLEPP_COVERAGE  "Build with code coverage options")
option(CONSOLEPP_SANITIZE "Build using sanitizers" "")
message("Building Server++ with config: ${CMAKE_BUILD_TYPE}")
message("Building Server++ with tests: ${CONSOLEPP_WITH_TESTS}")
message("Building Server++ with code coverage: ${CONSOLEPP_COVERAGE}")
message("Building Server++ with sanitizers: ${CONSOLEPP_SANITIZE}")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS NO_OUTPUT_DIRS)

# The required C++ Standard for Server++ is C++14.
set(CMAKE_CXX_STANDARD 14)

# When building shared objects, etc., we only want to export certain symbols.
# Therefore, we need to generate a header suitable for declaring which
# symbols should be included.
include(GenerateExportHeader)

# For producing automatically-generated documentation, we use Doxygen.
find_package(Doxygen)

if (CONSOLEPP_SANITIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${CONSOLEPP_SANITIZE}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=${CONSOLEPP_SANITIZE}")
endif()

if (CONSOLEPP_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -O0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

set (CONSOLEPP_PUBLIC_SOURCE_FILES
    src/console.cpp
)

set (CONSOLEPP_PUBLIC_HEADER_FILES
    include/consolepp/console.hpp
    include/consolepp/version.hpp
)

set (CONSOLEPP_PRIVATE_SOURCE_FILES
)

set (CONSOLEPP_PRIVATE_HEADER_FILES
)

add_library(consolepp
    ${CONSOLEPP_PUBLIC_HEADER_FILES}
    ${CONSOLEPP_PUBLIC_SOURCE_FILES}
    ${CONSOLEPP_PRIVATE_SOURCE_FILES}
    ${CONSOLEPP_PRIVATE_INCLUDE_FILES}
)

target_link_libraries(consolepp
    PUBLIC
        CONAN_PKG::gsl-lite
        CONAN_PKG::boost_asio
)

set_target_properties(consolepp
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VERSION ${CONSOLEPP_VERSION}
        SOVERSION ${CONSOLEPP_VERSION}
)

target_include_directories(consolepp
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/consolepp-${CONSOLEPP_VERSION}>
        "${Boost_INCLUDE_DIRS}"
)

generate_export_header(consolepp
    EXPORT_FILE_NAME "${PROJECT_SOURCE_DIR}/include/consolepp/detail/export.hpp"
)

configure_file(
    ${PROJECT_SOURCE_DIR}/include/consolepp/version.hpp.in
    ${PROJECT_SOURCE_DIR}/include/consolepp/version.hpp
    @ONLY)

install(
    TARGETS
        consolepp
    EXPORT
        consolepp-config
    ARCHIVE DESTINATION
        lib/consolepp-${CONSOLEPP_VERSION}
    LIBRARY DESTINATION
        lib/consolepp-${CONSOLEPP_VERSION}
)

install(
    DIRECTORY
        include/
    DESTINATION
        include/consolepp-${CONSOLEPP_VERSION}
)

export(
    EXPORT
        consolepp-config
    FILE
        "${CMAKE_CURRENT_BINARY_DIR}/consolepp-config.cmake"
)

install(
    EXPORT
        consolepp-config
    DESTINATION
        lib/consolepp-${CONSOLEPP_VERSION}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/consolepp-config-version.cmake"
    VERSION
        "${CONSOLEPP_VERSION}"
    COMPATIBILITY AnyNewerVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/consolepp-config-version.cmake"
    DESTINATION
        lib/consolepp-${CONSOLEPP_VERSION}
)

if (CONSOLEPP_WITH_TESTS)
    enable_testing()

    set(consolepp_tester_tests
        test/dummy.cpp
    )

    add_executable(consolepp_tester
        ${consolepp_tester_tests}
    )

    target_link_libraries(consolepp_tester
        PRIVATE
            consolepp
            CONAN_PKG::gtest
    )

    add_test(consolepp_test consolepp_tester)
endif()

# Add a rule for generating documentation
if (DOXYGEN_FOUND)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY)

    add_custom_target(consolepp_doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT
            "Generate API documentation with Doxygen" VERBATIM
    )
endif()

# Add customizations for packaging
set(CPACK_PACKAGE_NAME "Server++")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Server++")
set(CPACK_PACKAGE_VENDOR "Matthew Chaplain")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR ${CONSOLEPP_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CONSOLEPP_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CONSOLEPP_VERSION_PATCH})
include(CPack)
