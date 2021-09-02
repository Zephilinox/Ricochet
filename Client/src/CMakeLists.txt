include(ConfigureTarget)
include(CompilerWarnings)
include(FetchIMGUI)

add_library(client OBJECT
    "Client/Game.hpp"
    "Client/Game.cpp"
)
add_library(rico::client ALIAS client)

target_link_libraries(client PUBLIC core::core)
target_include_directories(client PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_link_options(client PUBLIC "-sUSE_SDL=2" "-sUSE_FREETYPE=1")
else()
    if (UNIX AND NOT APPLE)
        target_link_libraries(client PUBLIC "m" "atomic" "pthread")
    endif ()
endif()

configure_target(client "${CLIENT_CODE_COVERAGE}")
set_target_warnings(client "${CLIENT_WARNINGS_AS_ERRORS}")

add_executable(client_executable "main.cpp")
target_link_libraries(client_executable PUBLIC client)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set_target_properties(
        client_executable
        PROPERTIES 
            SUFFIX ".html" #change to ".wasm" if you only want the wasm output
    )
endif()

configure_target(client_executable "${CLIENT_CODE_COVERAGE}")
set_target_warnings(client_executable "${CLIENT_WARNINGS_AS_ERRORS}")