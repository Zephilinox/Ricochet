cmake_minimum_required(VERSION 3.21)

project(client)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

include(CheckIsRoot)
check_project_is_root(CLIENT_IS_ROOT)

add_subdirectory("../Core" "${CMAKE_CURRENT_BINARY_DIR}/Core")

option(CLIENT_CI "We are running on CI. May change certain defaults." OFF)
if (CLIENT_CI)
    message(STATUS "Client running on CI")
endif()

if (CLIENT_IS_ROOT)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
        set(CLIENT_IS_ROOT_AND_NOT_EMS OFF)
    else()
        set(CLIENT_IS_ROOT_AND_NOT_EMS ON)
    endif()
else()
    set(CLIENT_IS_ROOT_AND_NOT_EMS OFF)
endif()

if (NOT CPM_SOURCE_CACHE)
    set(CPM_SOURCE_CACHE "${CMAKE_CURRENT_SOURCE_DIR}/../.cpmcache")
endif()

# todo: don't override the users choice
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CLIENT_TESTS "Build the client tests" ${CLIENT_IS_ROOT_AND_NOT_EMS})
option(CLIENT_BENCHMARKS "Build the client benchmarks" ${CLIENT_IS_ROOT_AND_NOT_EMS})
option(CLIENT_INSTALL "Enable cmake install for the client" ${CLIENT_IS_ROOT})
option(CLIENT_DOWNLOAD_CPM "Download CPM. Disable if you're providing your own version of CPM" ON)
option(CLIENT_CLANG_FORMAT_CHECK "Adds the client_clang_format_check target" OFF)
option(CLIENT_CLANG_FORMAT_FIX "Adds the client_clang_format_fix target" OFF)
option(CLIENT_CPPCHECK "Adds the client_cppcheck target" OFF)
option(CLIENT_CLANG_TIDY_CHECK "Adds the client_clang_tidy_check target" OFF)
option(CLIENT_ASAN "Build with ASAN" OFF)
option(CLIENT_LSAN "Build with LSAN" OFF)
option(CLIENT_TSAN "Build with TSAN" OFF)
option(CLIENT_UBSAN "Build with UBSAN" OFF)
option(CLIENT_CODE_COVERAGE "Enable code coverage" OFF)
option(CLIENT_WARNINGS_AS_ERRORS "Treat warnings as errors" ${CLIENT_CI})
option(CLIENT_VALGRIND "Use valgrind to run all executables" OFF)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../dependencies" "${CMAKE_CURRENT_BINARY_DIR}/dependencies")

if (CLIENT_IS_ROOT)
    message(STATUS "Client is root")
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

if (CLIENT_DOWNLOAD_CPM)
    message(STATUS "Client CPM Download Enabled")
    include(DownloadCPM)
endif()

if (CLIENT_ASAN)
    message(STATUS "Client ASAN Enabled")
    include(SanitizeAddress)
endif()

if (CLIENT_LSAN)
    message(STATUS "Client LSAN Enabled")
    include(SanitizeLeaks)
endif()

if (CLIENT_TSAN)
    message(STATUS "Client TSAN Enabled")
    include(SanitizeThreads)
endif()

if (CLIENT_UBSAN)
    message(STATUS "Client UBSAN Enabled")
    include(SanitizeUndefinedBehaviour)
endif()

if (CLIENT_VALGRND)
    message(STATUS "Client Valgrind Enabled")
    include(Valgrind)
endif()

add_subdirectory("src")

if (CLIENT_INSTALL)
    message(STATUS "Client Install Enabled")
    install(
        DIRECTORY "$<TARGET_FILE_DIR:client_executable>"
        DESTINATION "."
    )
endif()

if (CLIENT_TESTS)
    message(STATUS "Client Tests Enabled")
    enable_testing()
    add_subdirectory(tests)
endif()

if (CLIENT_BENCHMARKS)
    message(STATUS "Client Benchmarks Enabled")
    add_subdirectory(benchmarks)
endif()

set(CLIENT_LINT_PATHS
    "${CMAKE_CURRENT_SOURCE_DIR}/tests"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks"
)

if (CLIENT_CLANG_FORMAT_CHECK)
    message(STATUS "Client clang-format check Enabled")
    include(ClangFormat)
    create_clang_format_check(
        "client_clang_format_check"
        "${CLIENT_LINT_PATHS}"
    )
endif()

if (CLIENT_CLANG_FORMAT_FIX)
    message(STATUS "Client clang-format fix Enabled")
    include(ClangFormat)
    create_clang_format_fix(
        "client_clang_format_fix"
        "${CLIENT_LINT_PATHS}"
    )
endif()

if (CLIENT_CPPCHECK)
    message(STATUS "Client cppcheck Enabled")
    include(CPPCheck)
    cppcheck_sources(client_cppcheck
        "Client/"
        "${CLIENT_LINT_PATHS}"
    )
endif()

if (CLIENT_CLANG_TIDY_CHECK)
    message(STATUS "Client clang-tidy check Enabled")
    include(ClangTidy)
    create_clang_tidy_check(client_clang_tidy_check "Client/")
endif()