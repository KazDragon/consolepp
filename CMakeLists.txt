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

option(CONOSOLEPP_COVERAGE  "Build with code coverage options")
option(CONSOLEPP_SANITIZE "Build using sanitizers" "")
option(CONSOLEPP_WITH_TESTS "Build with tests" True)
option(CONSOLEPP_DOC_ONLY "Build only documentation" False)

message("Building Console++ with build type: ${CMAKE_BUILD_TYPE}")
message("Building Console++ with code coverage: ${CONSOLEPP_COVERAGE}")
message("Building Console++ with sanitizers: ${CONSOLEPP_SANITIZE}")
message("Building Console++ with tests: ${CONSOLEPP_WITH_TESTS}")
message("Building Console++ with only documentation: ${CONSOLEPP_DOC_ONLY}")

# For producing automatically-generated documentation, we use Doxygen.
find_package(Doxygen)

# Add a rule for generating documentation
if (DOXYGEN_FOUND)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY)

    add_custom_target(CONSOLEPP_doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT
            "Generate API documentation with Doxygen" VERBATIM
    )
endif()

if (CONSOLEPP_DOC_ONLY)
    return()
endif()

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    message("Using Conan due to conanbuildinfo.cmake")
    set(CONSOLEPP_USE_CONAN ON)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
endif()

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# The required C++ Standard for Console++ is C++14.
set(CMAKE_CXX_STANDARD 14)

if (CONSOLEPP_USE_CONAN)
    set(CONSOLEPP_PUBLIC_LIBRARIES
        CONAN_PKG::boost
        CONAN_PKG::gsl-lite
    )

    set(CONSOLEPP_PRIVATE_LIBRARIES)
    set(CONSOLEPP_TESTER_LIBRARIES
        consolepp
        CONAN_PKG::gtest
    )
else()
    # If we are building with tests, then we require the GTest library
    if (${CONSOLEPP_WITH_TESTS})
        find_package(GTest REQUIRED)
    endif()

    # Console++ requires at least Boost 1.69.
    find_package(Boost 1.69.0 REQUIRED COMPONENTS container)

    # Console++ requires exactly version 0.34 of gsl-lite
    find_package(gsl-lite 0.34.0 EXACT REQUIRED)

    set(CONSOLEPP_PUBLIC_LIBRARIES
        Boost::boost
        gsl::gsl-lite
    )

    set(CONSOLEPP_PRIVATE_LIBRARIES)
    set(CONSOLEPP_TESTER_LIBRARIES
        consolepp
        GTest::GTest
        GTest::Main)
endif()

# When building shared objects, etc., we only want to export certain symbols.
# Therefore, we need to generate a header suitable for declaring which
# symbols should be included.
include(GenerateExportHeader)

if (CONSOLEPP_SANITIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${CONSOLEPP_SANITIZE}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=${CONSOLEPP_SANITIZE}")
endif()

if (CONSOLEPP_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -O0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

add_library(consolepp)

target_sources(consolepp
    PRIVATE
        include/consolepp/console.hpp

        src/console.cpp
)

target_link_libraries(consolepp
    PUBLIC
        ${CONSOLEPP_PUBLIC_LIBRARIES}
    PRIVATE
        ${CONSOLEPP_PRIVATE_LIBRARIES}
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

    add_executable(consolepp_tester)

    target_sources(consolepp_tester
        PRIVATE
            test/consolepp-test.cpp
    )

    target_link_libraries(consolepp_tester
        PRIVATE
            ${CONSOLEPP_TESTER_LIBRARIES}
    )

    add_test(consolepp_test consolepp_tester)
endif()

# Add customizations for packaging
set(CPACK_PACKAGE_NAME "Console++")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Console++")
set(CPACK_PACKAGE_VENDOR "Matthew Chaplain")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR ${CONSOLEPP_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CONSOLEPP_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CONSOLEPP_VERSION_PATCH})
include(CPack)
