include(FetchGoogleBenchmark)
include(ConfigureTarget)
include(CompilerWarnings)

add_executable(core_benchmarks "main.cpp"
    "Signal.bench.hpp"
)

configure_target(core_benchmarks "${CORE_CODE_COVERAGE}")
set_target_warnings(core_benchmarks "${CORE_WARNINGS_AS_ERRORS}")

target_link_libraries(core_benchmarks
    PRIVATE
        core
        benchmark::benchmark
)

# Don't use the crosscompiling executor, if it's valgrind they will be very slow.
# We'll rely on static analysis for benchmark leaks. in theory, tests should cover it.
# Also, googlebenchmark has an intentional memory leak, so we'd need to supress it.
add_custom_target(core_benchmarks_execute
    COMMAND "$<TARGET_FILE:core_benchmarks>"
)