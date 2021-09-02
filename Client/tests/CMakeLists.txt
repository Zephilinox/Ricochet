include(FetchDoctest)
include(ConfigureTarget)
include(CompilerWarnings)

add_executable(client_tests
    "main.cpp"
    "client.hpp"
)

configure_target(client_tests "${CLIENT_CODE_COVERAGE}")
set_target_warnings(client_tests "${CLIENT_WARNINGS_AS_ERRORS}")

target_link_libraries(client_tests
    PRIVATE
        client
        doctest::doctest
)

include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")
#doctest_discover_tests(client_tests JUNIT_OUTPUT_DIR ".")
doctest_discover_tests(client_tests)