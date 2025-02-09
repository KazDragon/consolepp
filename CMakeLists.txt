if (POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
    project(CONSOLEPP VERSION 0.1.1)
else()
    project(CONSOLEPP)
endif()

cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

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

    add_custom_target(consolepp_doc
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

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# If we are building with tests, then we require the GTest library
if (${CONSOLEPP_WITH_TESTS})
    find_package(GTest REQUIRED)
endif()

# Console++ requires at least Boost 1.69.
find_package(Boost 1.69.0 REQUIRED)

# Console++ requires at least version 0.38 of gsl-lite
find_package(gsl-lite 0.38.0 REQUIRED)

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
add_library(KazDragon::consolepp ALIAS consolepp)

target_sources(consolepp
    PRIVATE
        include/consolepp/detail/export.hpp
        include/consolepp/console.hpp
        include/consolepp/core.hpp
        include/consolepp/version.hpp

        src/console.cpp
)

target_link_libraries(consolepp
    PUBLIC
        Boost::boost
        gsl::gsl-lite
)

target_include_directories(consolepp
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/consolepp-${CONSOLEPP_VERSION}>
)

set_target_properties(consolepp
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VERSION ${CONSOLEPP_VERSION}
        SOVERSION ${CONSOLEPP_VERSION}
        DEBUG_POSTFIX "d"
)

target_compile_features(consolepp
    PUBLIC
        cxx_std_20
)

target_compile_options(consolepp
    PRIVATE
        # Do not generate warning C4251 (member needs dll linkage) on MSVC
        $<$<CXX_COMPILER_ID:MSVC>:/wd4251>
        # Do not generate warning C4275 (type needs dll linkage) on MSVC
        $<$<CXX_COMPILER_ID:MSVC>:/wd4275>
        # Any warning on MSVC is an error
        $<$<CXX_COMPILER_ID:MSVC>:/WX>

        # Add warnings on g++ and Clang
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wpedantic -Wsign-conversion -Werror -Wno-unused-parameter>
)

generate_export_header(consolepp
    EXPORT_FILE_NAME "${PROJECT_SOURCE_DIR}/include/consolepp/detail/export.hpp"
)

configure_file(
    ${PROJECT_SOURCE_DIR}/include/consolepp/version.hpp.in
    ${PROJECT_SOURCE_DIR}/include/consolepp/version.hpp
    @ONLY)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/consolepp-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/consolepp-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/consolepp
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/consolepp-config-version.cmake"
    VERSION
        "${CONSOLEPP_VERSION}"
    COMPATIBILITY AnyNewerVersion
)

export(
    TARGETS
        consolepp
    NAMESPACE
        KazDragon::
    FILE ${PROJECT_BINARY_DIR}/consolepp-targets.cmake
)

install(
    TARGETS
        consolepp
    EXPORT
        consolepp-targets
    ARCHIVE DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/consolepp-${CONSOLEPP_VERSION}
    LIBRARY DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/consolepp-${CONSOLEPP_VERSION}
    RUNTIME DESTINATION
        ${CMAKE_INSTALL_BINDIR}/consolepp-${CONSOLEPP_VERSION}
)

install(
    EXPORT
        consolepp-targets
    NAMESPACE 
        KazDragon::
    DESTINATION
        ${CMAKE_INSTALL_DATADIR}/consolepp-${CONSOLEPP_VERSION}
)

install(
    DIRECTORY
        include/
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/consolepp-${CONSOLEPP_VERSION}
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/consolepp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/consolepp-config-version.cmake"
    DESTINATION
        ${CMAKE_INSTALL_DATADIR}/consolepp-${CONSOLEPP_VERSION}
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
            consolepp
            GTest::GTest
            GTest::Main
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
