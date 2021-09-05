#pragma once

//SELF

//LIBS
#include <Core/Window/Window.hpp>
#include <Core/Input/Input.hpp>
#include <Core/IMGUI/IMGUI.hpp>
#include <Core/Raytrace/Ray.hpp>
#include <Core/Raytrace/Scene.hpp>
#include <Core/Raytrace/Frame.hpp>
#include <Core/Raytrace/Camera.hpp>
#include <Core/Raytrace/Shape/Shape.hpp>
#include <Core/Raytrace/Material/Material.hpp>
#include <Core/Texture/TextureGL.hpp>
#include <threadpool/threadpool.hpp>

//STD
#include <array>

namespace core
{

class WindowSDL;

}

namespace rico::client
{

class Game
{
public:
    static int add(int, int);

    Game();
    ~Game();

    int run();

    void events();
    void update();
    void render();

private:
    void raytrace_chunk(std::size_t start, std::size_t end);
    void raytrace();

    std::unique_ptr<core::Window> window;
    core::WindowSDL* window_sdl = nullptr; //todo: not needed when we have proper abstractions everywhere
    void* glc;                             //todo: hide in renderer?
    core::Input input;

    //todo: remove once we have some IMGUI windows we can use instead, or maybe hide behind a special key
    bool show_demo_window = true;

    //todo: lots of openGL specific stuff, that should live... somewhere else
    std::array<float, 6> vertices{ 0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f };
    const char* vertexSource = "attribute vec4 position;    \n"
                               "void main()                  \n"
                               "{                            \n"
                               "   gl_Position = vec4(position.xyz, 1.0);  \n"
                               "}                            \n";
    const char* fragmentSource = "precision mediump float;\n"
                                 "void main()                                  \n"
                                 "{                                            \n"
                                 "  gl_FragColor = vec4 (1.0, 1.0, 1.0, 1.0 );\n"
                                 "}                                            \n";

    std::unique_ptr<core::IMGUI> imgui;

    // Raytracing
    core::Camera camera;
    core::Frame render_frame;
    core::Frame update_frame;
    core::Scene scene;
    std::unique_ptr<core::TextureGL> texture;
    zx::threadpool<> pool;
    std::mutex super_cool_mutex;
    std::mutex super_cool_mutex2;
    std::atomic<float> percentage = 0.0f;
    std::size_t max_samples = 1;
    std::atomic<int> current_samples = 0;
    std::thread raytracer_thread;
    std::atomic<bool> raytracing = true;

    core::Sphere sphere1;
    core::Sphere sphere2;
    core::Sphere sphere3;
    core::Sphere sphere4;
};

} // namespace rico::client