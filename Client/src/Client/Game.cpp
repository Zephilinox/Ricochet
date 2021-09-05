#include "Game.hpp"

//SELF

//LIBS
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>

    #define GL_GLEXT_PROTOTYPES 1
    #include <SDL_opengles2.h>
#else
    #undef main
    #include <glad/glad.h>
#endif

#include <Core/Window/WindowSDL.hpp>
#include <Core/IMGUI/IMGUI_SDL_GL.hpp>
#include <Core/Raytrace/Shape/Shape.hpp>
#include <Core/Raytrace/Material/Material.hpp>

#include <cstring> //memcpy for SDL
#include <SDL.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

using namespace rico::client;

//STD
#include <fstream>

int Game::add(int a, int b)
{
    return a + b;
}

inline void magic_function()
{
    auto err = glGetError();
    if (err != GL_NO_ERROR)
    {
        //spdlog::info("OpenGL error - {:x}", err);
        int totes_important = 0;
    }
}

inline glm::highp_dvec3 cast_ray(const core::Ray& ray, const core::Scene& scene, int depth)
{
	//const auto unit_dir = glm::normalize(ray.direction);
	//const auto t = 0.5 * (unit_dir.y + 1.0);
	//return (1.0f - t) * glm::highp_dvec3(1.0, 1.0, 1.0) + t * glm::highp_dvec3(0.5, 0.7, 1.0);
    
    auto maybe_hit = scene.hit(ray, 0.00001, 999.9);
	if (maybe_hit)
    {
		if (depth >= 100)
            return {0.0, 0.0, 0.0};

        auto maybe_bounce = maybe_hit->material->bounce(ray, 0.0001, 999.9, *maybe_hit);
		if (maybe_bounce)
			return maybe_bounce->attenuation * cast_ray(maybe_bounce->ray, scene, depth + 1) * maybe_bounce->absorption;

		return maybe_hit->colour;
	}
    
	const auto unit_dir = glm::normalize(ray.direction);
	const auto t = 0.5 * (unit_dir.y + 1.0);
	return (1.0 - t) * glm::highp_dvec3(1.0, 1.0, 1.0) + t * glm::highp_dvec3(0.5, 0.7, 1.0);
}

Game::Game()
    : frame(camera.width, camera.height)
{
    //todo: hide somewhere, must be done before anything SDL related
    SDL_Init(SDL_INIT_VIDEO);

    //todo: hide somewhere, must be done before making window, but after knowing what renderer we want
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    core::Window::Settings settings{ 1600, 900, "Client" };
    window = std::make_unique<core::WindowSDL>(settings);
    //todo: once we've abstracted everything, we shouldn't need to do this.
    window_sdl = window->as<core::WindowSDL>();

    //todo: hide in renderer
    glc = SDL_GL_CreateContext(window_sdl->getRawWindow());
    //todo: currently the window has settings for vsync, have the renderer use those to set vsync when it's created? or move vsync to the renderer?
    SDL_GL_SetSwapInterval(0); // VSync off, though when running in a browser it may get overriden

    //todo: hide in renderer
    if (!static_cast<bool>(gladLoadGLES2Loader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress))))
    {
        spdlog::error("Failed to initialize OpenGLES loader!\n");
        //todo: how to handle this failing (nicely?)
        throw;
    }
    magic_function();

    spdlog::info("OpenGLES version loaded: {}.{}\n", GLVersion.major, GLVersion.minor);

    imgui = std::make_unique<core::IMGUI_SDL_GL>(*window, glc);
    magic_function();

    //todo: this is nasty
    {
        // Create Vertex Array Object
        GLuint vao = 0;
        glGenVertexArraysOES(1, &vao);
        magic_function();
        glBindVertexArrayOES(vao);
        magic_function();

        // Create a Vertex Buffer Object and copy the vertex data to it
        GLuint vbo = 0;
        glGenBuffers(1, &vbo);
        magic_function();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        magic_function();
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(GLfloat)), vertices.data(), GL_STATIC_DRAW);
        magic_function();

        // Create and compile the vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        magic_function();
        glShaderSource(vertexShader, 1, &vertexSource, nullptr);
        magic_function();
        glCompileShader(vertexShader);
        magic_function();

        // Create and compile the fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        magic_function();
        glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
        magic_function();
        glCompileShader(fragmentShader);
        magic_function();

        // Link the vertex and fragment shader into a shader program
        GLuint shaderProgram = glCreateProgram();
        magic_function();
        glAttachShader(shaderProgram, vertexShader);
        magic_function();
        glAttachShader(shaderProgram, fragmentShader);
        magic_function();
        // glBindFragDataLocation(shaderProgram, 0, "outColor");
        glLinkProgram(shaderProgram);
        magic_function();
        glUseProgram(shaderProgram);
        magic_function();

        // Specify the layout of the vertex data
        GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
        magic_function();
        glEnableVertexAttribArray(static_cast<GLuint>(posAttrib));
        magic_function();
        glVertexAttribPointer(static_cast<GLuint>(posAttrib), 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        magic_function();
    }

    texture = std::make_unique<core::TextureGL>();
    std::vector<core::Pixel> pixels;
    pixels.resize(frame.pixels_width * frame.pixels_height);
    texture->load(pixels, frame.pixels_width, frame.pixels_height);
    magic_function();
}

Game::~Game()
{
    imgui.reset();

    SDL_GL_DeleteContext(glc); //todo: let renderer do it
    window.reset();
    SDL_Quit(); //todo: let something else do it. not sure if window is correct, what if we end up wanting multiple windows?
    magic_function();
}

int Game::run()
{
    auto diffuse = std::make_unique<core::Diffuse>();
    auto sphere = std::make_unique<core::Sphere>();
    sphere->center = glm::highp_dvec3(0.0f, 0.0f, -1.0f);
    sphere->radius = 0.5f;
    sphere->colour = glm::highp_dvec3(0.8f, 0.8f, 0.8f);
    sphere->material = diffuse.get();
    scene.hitables.emplace_back(std::move(sphere));
    auto sphere2 = std::make_unique<core::Sphere>();
    sphere2->center = glm::highp_dvec3(0.0f, -100.5f, -1.0f);
    sphere2->radius = 100.0f;
    sphere2->colour = glm::highp_dvec3(0.8f, 0.8f, 0.8f);
    sphere2->material = diffuse.get();
    scene.hitables.emplace_back(std::move(sphere2));

    std::vector<std::thread> threads;
    std::mutex super_cool_mutex;
    auto lambda = [&](std::size_t thread_id, std::size_t start, std::size_t end) {
        std::vector<glm::vec3> our_pixels;
        our_pixels.resize(end - start);

        std::this_thread::sleep_for(std::chrono::seconds(1));
        for (std::size_t i = start; i < end; ++i)
        {        
		    glm::highp_dvec3 colour(0.0, 0.0, 0.0);
            constexpr auto max_samples = 512; //todo: use zx::threadpool and multithread the samples
            for (std::size_t sample = 0; sample < max_samples; ++sample)
            {
                const float rand_u = static_cast<float>(std::rand() % 10000) / 10000.0f;
                const float rand_v = static_cast<float>(std::rand() % 10000) / 10000.0f;

                const double x = i % frame.pixels_width;
                const double y = i / frame.pixels_width;
                if (y == 2)
                    int aihfiahfas = 0;
                const double u = (x + rand_u) / static_cast<double>(frame.pixels_width - 1);
                const double v = 1.0 - ((y + rand_v) / static_cast<double>(frame.pixels_height - 1));
                const core::Ray ray{
                    camera.origin,
                    camera.lower_left_corner + (u * camera.horizontal) + (v * camera.vertical) - camera.origin
                };

                const auto sample_colour = cast_ray(ray, scene, 0);
                if (sample_colour.r != -1.0)
                    int hajfhasf = 0;
            
                if (sample_colour == glm::highp_dvec3(1.0))
                    int aisdhfahsf = 0;
                colour += sample_colour;
            }
            our_pixels[i - start] = colour / static_cast<double>(max_samples);
        }

        {
            std::scoped_lock lock(super_cool_mutex);
            for (std::size_t i = 0; i < our_pixels.size(); ++i)
            {
                frame.pixels[i + start] = our_pixels[i];
            }
            spdlog::info("thread done ({}%)", (static_cast<float>(end - start) / static_cast<float>(frame.pixels.size())) * 100.0f);
        }
    };

    for (int i = 0; i < (std::thread::hardware_concurrency() * 2); ++i)
    {
        const auto chunk = frame.pixels.size() / (std::thread::hardware_concurrency() * 2);
        if (i == (std::thread::hardware_concurrency() * 2) - 1)
            threads.emplace_back(lambda, i, chunk * i, frame.pixels.size());
        else
            threads.emplace_back(lambda, i, chunk * i, chunk * (i + 1));
    }

    for (auto& t : threads)
    {
        //t.join();
    }

    magic_function();
    while (window->isOpen())
    {
        int i2 = 0;
        magic_function();
        events();
        magic_function();
        update();
        magic_function();
        render();
        magic_function();
        int i = 0;
    }

    return 0;
}

void Game::events()
{
    input.prepare();

    core::Event event;
    while (window->poll(event))
    {
        auto consumed = imgui->process(event);
        if (consumed)
            continue;

        input.process(event);
        auto visitor = core::overload{
            [&](core::EventQuit&) {
                spdlog::info("Closing Client");
                window->close();
            },
            [](auto&&) {

            }
        };
        std::visit(visitor, event);
    }
}

void Game::update()
{
    // IMGUI needs to know when the frame started
    imgui->update(*window);

    // Raytrace
    //todo
    
    // UI
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Begin("FPS");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / static_cast<double>(ImGui::GetIO().Framerate), static_cast<double>(ImGui::GetIO().Framerate));
    ImGui::End();

    ImGui::Begin("Raytrace");
    ImGui::Image((void*)(intptr_t)texture->getOpenglTextureID(), ImVec2(texture->getWidth(), texture->getHeight()));
    ImGui::End();
}

void Game::render()
{
    magic_function();
    //todo: where should this live? in the renderer? do we need to use SDL to make the GL context current? can we use the GL API directly?
    SDL_GL_MakeCurrent(window_sdl->getRawWindow(), glc);
    magic_function();

    // Clear the screen. todo: move to window (like SFML) or the renderer (GL specific?)
    magic_function();
    glClearColor(1.0f, 0.7f, 0.0f, 1.0f);
    magic_function();
    glClear(GL_COLOR_BUFFER_BIT);
    magic_function();

    // Adjust the viewport in case it is resized
    //todo: this is grabbing it from IMGUI, but we can just grab it from the window.
    //      have the renderer process window resize events or something? or does it need to be done exactly here?
    glViewport(0, 0, static_cast<int>(imgui->getRawIMGUI()->DisplaySize.x), static_cast<int>(imgui->getRawIMGUI()->DisplaySize.y));
    magic_function();

    // Draw a triangle from the 3 vertices, underneath IMGUI
    //todo: renderer stuff innit
    glDrawArrays(GL_TRIANGLES, 0, 3);
    magic_function();

    texture->update([&](std::vector<core::Pixel>& pixels) {
        frame.toPixels(pixels);
    });
    magic_function();

    imgui->render();
    magic_function();

    //todo: seems like window should do this, but then it's GL specific? so renderer?
    SDL_GL_SwapWindow(window_sdl->getRawWindow());
    magic_function();
}
