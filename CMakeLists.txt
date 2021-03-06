cmake_minimum_required(VERSION 3.21)

project(ricochet)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CheckIsRoot)
check_project_is_root(RICO_IS_ROOT)

if (NOT RICO_IS_ROOT)
    message(SEND_ERROR "Root CMakeLists.txt is for development only")
endif()

find_program(ClangTidy clang-tidy)
set(USE_CLANG_TIDY ON)
if (NOT ClangTidy)
    set(USE_CLANG_TIDY OFF)
endif()

find_program(ClangFormat clang-format)
set(USE_CLANG_FORMAT ON)
if (NOT ClangFormat)
    set(USE_CLANG_FORMAT OFF)
endif()

find_program(CPPCheck cppcheck)
set(USE_CPPCHECK ON)
if (NOT CPPCheck)
    set(USE_CPPCHECK OFF)
endif()

function(set_vars_and_add project_name project_name_caps)
    set("${project_name_caps}_TESTS" "${RICO_IS_ROOT}")
    set("${project_name_caps}_BENCHMARKS" "${RICO_IS_ROOT}")
    set("${project_name_caps}_CLANG_TIDY_CHECK" "${USE_CLANG_TIDY}")
    set("${project_name_caps}_CLANG_FORMAT_CHECK" "${USE_CLANG_FORMAT}")
    set("${project_name_caps}_CLANG_FORMAT_FIX" "${USE_CLANG_FORMAT}")
    set("${project_name_caps}_CPPCHECK" "${USE_CPPCHECK}")
    add_subdirectory("${project_name}")
endfunction()

set_vars_and_add("Core" "CORE")
set_vars_and_add("Client" "CLIENT")