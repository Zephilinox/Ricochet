if (TARGET core)
    return()
endif()

cmake_minimum_required(VERSION 3.21)

project(core)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

include(CheckIsRoot)
check_project_is_root(CORE_IS_ROOT)

option(CORE_CI "We are running on CI. May change certain defaults." OFF)
if (CORE_CI)
    message(STATUS "Core running on CI")
endif()

if (CORE_IS_ROOT)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
        set(CORE_IS_ROOT_AND_NOT_EMS OFF)
    else()
        set(CORE_IS_ROOT_AND_NOT_EMS ON)
    endif()
else()
    set(CORE_IS_ROOT_AND_NOT_EMS OFF)
endif()

if (NOT CPM_SOURCE_CACHE)
    set(CPM_SOURCE_CACHE "${CMAKE_CURRENT_SOURCE_DIR}/../.cpmcache")
endif()

# todo: don't override the users choice
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CORE_TESTS "Build the core tests" ${CORE_IS_ROOT_AND_NOT_EMS})
option(CORE_BENCHMARKS "Build the core benchmarks" ${CORE_IS_ROOT_AND_NOT_EMS})
option(CORE_DOWNLOAD_CPM "Download CPM. Disable if you're providing your own version of CPM" ON)
option(CORE_CLANG_FORMAT_CHECK "Adds the core_clang_format_check target" OFF)
option(CORE_CLANG_FORMAT_FIX "Adds the core_clang_format_fix target" OFF)
option(CORE_CPPCHECK "Adds the core_cppcheck target" OFF)
option(CORE_CLANG_TIDY_CHECK "Adds the core_clang_tidy_check target" OFF)
option(CORE_ASAN "Build with ASAN" OFF)
option(CORE_LSAN "Build with LSAN" OFF)
option(CORE_TSAN "Build with TSAN" OFF)
option(CORE_UBSAN "Build with UBSAN" OFF)
option(CORE_CODE_COVERAGE "Enable code coverage" OFF)
option(CORE_WARNINGS_AS_ERRORS "Treat warnings as errors" ${CORE_CI})
option(CORE_VALGRIND "Use valgrind to run all executables" OFF)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../dependencies" "${CMAKE_CURRENT_BINARY_DIR}/dependencies")

if (CORE_IS_ROOT)
    message(STATUS "Core is root")
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

if (CORE_DOWNLOAD_CPM)
    message(STATUS "Core CPM Download Enabled")
    include(DownloadCPM)
endif()

if (CORE_ASAN)
    message(STATUS "Core ASAN Enabled")
    include(SanitizeAddress)
endif()

if (CORE_LSAN)
    message(STATUS "Core LSAN Enabled")
    include(SanitizeLeaks)
endif()

if (CORE_TSAN)
    message(STATUS "Core TSAN Enabled")
    include(SanitizeThreads)
endif()

if (CORE_UBSAN)
    message(STATUS "Core UBSAN Enabled")
    include(SanitizeUndefinedBehaviour)
endif()

if (CORE_VALGRND)
    message(STATUS "Core Valgrind Enabled")
    include(Valgrind)
endif()

add_subdirectory("src")

if (CORE_TESTS)
    message(STATUS "Core Tests Enabled")
    enable_testing()
    add_subdirectory(tests)
endif()

if (CORE_BENCHMARKS)
    message(STATUS "Core Benchmarks Enabled")
    add_subdirectory(benchmarks)
endif()

set(CORE_LINT_PATHS
    "${CMAKE_CURRENT_SOURCE_DIR}/tests"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks"
)

if (CORE_CLANG_FORMAT_CHECK)
    message(STATUS "Core clang-format check Enabled")
    include(ClangFormat)
    create_clang_format_check(
        "core_clang_format_check"
        "${CORE_LINT_PATHS}"
    )
endif()

if (CORE_CLANG_FORMAT_FIX)
    message(STATUS "Core clang-format fix Enabled")
    include(ClangFormat)
    create_clang_format_fix(
        "core_clang_format_fix"
        "${CORE_LINT_PATHS}"
    )
endif()

if (CORE_CPPCHECK)
    message(STATUS "Core cppcheck Enabled")
    include(CPPCheck)
    cppcheck_sources(core_cppcheck
        "Core/"
        "${CORE_LINT_PATHS}"
    )
endif()

if (CORE_CLANG_TIDY_CHECK)
    message(STATUS "Core clang-tidy check Enabled")
    include(ClangTidy)
    create_clang_tidy_check(core_clang_tidy_check "Core/")
endif()