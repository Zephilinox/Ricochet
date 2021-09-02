include(FetchDoctest)
include(ConfigureTarget)
include(CompilerWarnings)

add_executable(core_tests
    "main.cpp"
    "core.hpp"
    "Signal.test.hpp"
)

configure_target(core_tests ${CORE_CODE_COVERAGE})
set_target_warnings(core_tests ${CORE_WARNINGS_AS_ERRORS})

target_link_libraries(core_tests
    PRIVATE
        core
        doctest::doctest
)

include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")
#doctest_discover_tests(core_tests JUNIT_OUTPUT_DIR ".")
doctest_discover_tests(core_tests)