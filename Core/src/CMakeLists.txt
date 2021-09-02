include(ConfigureTarget)
include(CompilerWarnings)
include(FetchASIO)
include(FetchGLM)
include(FetchFMT)
include(FetchSPDLOG)
include(FetchMagicEnum)

add_library(core
    "Core/Window/Window.hpp"
    "Core/Window/WindowSDL.hpp"
    "Core/Window/WindowSDL.cpp"
    "Core/Window/WindowNone.hpp"
    "Core/Window/WindowNone.cpp"

    "Core/Input/Input.hpp"
    "Core/Input/Input.cpp"
    "Core/Input/Events.hpp"
    "Core/Input/Keys.hpp"
    "Core/Input/Keys.cpp"
    "Core/Input/KeysSDL.hpp"

    "Core/Signal/Connection.hpp"
    "Core/Signal/Connection.cpp"
    "Core/Signal/Disconnector.hpp"
    "Core/Signal/Signal.hpp"
    "Core/Signal/Slot.hpp"

    "Core/IMGUI/IMGUI.hpp"
    "Core/IMGUI/IMGUI_SDL_GL.hpp"
    "Core/IMGUI/IMGUI_SDL_GL.cpp"
)
add_library(core::core ALIAS core)

target_include_directories(core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_link_options(core PUBLIC "-sUSE_SDL=2" "-sUSE_FREETYPE=1")
else()
    include(FetchSDL2)
    find_package(OpenGL)

    # glad is available from a previous call to add_subdirectory("dependencies")
    target_link_libraries(core PUBLIC SDL2-static OpenGL::GL glad)

    if (UNIX AND NOT APPLE)
        target_link_libraries(core PUBLIC "m" "atomic" "pthread")
    endif ()
endif()

include(FetchIMGUI)
target_link_libraries(core
    PUBLIC
        fmt::fmt
        spdlog::spdlog
        magic_enum::magic_enum
        glm
        asio
        imgui
)
configure_target(core "${CORE_CODE_COVERAGE}")
set_target_warnings(core "${CORE_WARNINGS_AS_ERRORS}")