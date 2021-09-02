include(FetchGoogleBenchmark)
include(ConfigureTarget)
include(CompilerWarnings)

add_executable(client_benchmarks
    "main.cpp"
)

configure_target(client_benchmarks ${CLIENT_CODE_COVERAGE})
set_target_warnings(client_benchmarks ${CLIENT_WARNINGS_AS_ERRORS})

target_link_libraries(client_benchmarks
    PRIVATE
        client
        benchmark::benchmark
)

# Don't use the crosscompiling executor, if it's valgrind they will be very slow.
# We'll rely on static analysis for benchmark leaks. in theory, tests should cover it.
# Also, googlebenchmark has an intentional memory leak, so we'd need to supress it.
add_custom_target(client_benchmarks_execute
    COMMAND "$<TARGET_FILE:client_benchmarks>"
)