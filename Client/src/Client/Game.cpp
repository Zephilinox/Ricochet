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

#include <cstring> //memcpy for SDL
#include <SDL.h>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <fmt/format.h>

using namespace rico::client;

//STD
#include <fstream>
#include <algorithm>
#include <random>

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
    : render_frame(camera.width, camera.height)
    , update_frame(camera.width, camera.height)
    , pool(std::max(static_cast<int>(std::thread::hardware_concurrency()) - 1, 1)) // leave 1 thread for us
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
    pixels.resize(render_frame.pixels_width * render_frame.pixels_height);
    texture->load(pixels, render_frame.pixels_width, render_frame.pixels_height);
    magic_function();
}

Game::~Game()
{
    raytracing = false;
    if (raytracer_thread.joinable())
        raytracer_thread.join();

    imgui.reset();

    SDL_GL_DeleteContext(glc); //todo: let renderer do it
    window.reset();
    SDL_Quit(); //todo: let something else do it. not sure if window is correct, what if we end up wanting multiple windows?
    magic_function();
}

int Game::run()
{
    auto diffuse = std::make_unique<core::Diffuse>();
    auto metal = std::make_unique<core::Metal>();
    //metal->fuzz = 0.1f;

    {
        sphere1.center = glm::highp_dvec3(0.0f, -0.3f, -1.5f);
        sphere1.radius = 0.7f;
        sphere1.colour = glm::highp_dvec3(1.0f, 1.0f, 1.0f);
        sphere1.material = metal.get();
        
        auto sphere = std::make_unique<core::Sphere>(sphere1);
        scene.hitables.emplace_back(std::move(sphere));
    }
    {
        sphere2.center = glm::highp_dvec3(-1.0f, -0.2f, -1.0f);
        sphere2.radius = 0.5f;
        sphere2.colour = glm::highp_dvec3(0.0f, 1.0f, 0.4f);
        sphere2.material = diffuse.get();

        auto sphere = std::make_unique<core::Sphere>(sphere2);
        scene.hitables.emplace_back(std::move(sphere));
    }
    {
        sphere3.center = glm::highp_dvec3(1.0f, -0.1f, -1.0f);
        sphere3.radius = 0.5f;
        sphere3.colour = glm::highp_dvec3(1.0f, 0.4f, 0.0f);
        sphere3.material = diffuse.get();

        auto sphere = std::make_unique<core::Sphere>(sphere3);
        scene.hitables.emplace_back(std::move(sphere));
    }
    {
        sphere4.center = glm::highp_dvec3(0.8f, -0.1f, -0.9f);
        sphere4.radius = 0.3f;
        sphere4.colour = glm::highp_dvec3(1.0f, 1.0f, 1.0f);
        sphere4.material = metal.get();

        auto sphere = std::make_unique<core::Sphere>(sphere4);
        scene.hitables.emplace_back(std::move(sphere));
    }
    {
        auto sphere = std::make_unique<core::Sphere>();
        sphere->center = glm::highp_dvec3(0.0f, -100.5f, -1.0f);
        sphere->radius = 100.0f;
        sphere->colour = glm::highp_dvec3(1.0f, 1.0f, 1.0f);
        sphere->material = diffuse.get();
        scene.hitables.emplace_back(std::move(sphere));
    }

    magic_function();
    raytracer_thread = std::thread(&Game::raytrace, this);
    while (window->isOpen())
    {
        magic_function();
        events();
        magic_function();
        update();
        magic_function();
        render();
        magic_function();
    }

    raytracing = false;
    if (raytracer_thread.joinable())
        raytracer_thread.join();

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
    
    // UI
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::Begin("FPS");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / static_cast<double>(ImGui::GetIO().Framerate), static_cast<double>(ImGui::GetIO().Framerate));
    ImGui::End();

    ImGui::Begin(fmt::format("Raytrace {} samples###1", current_samples).c_str());
    ImGui::Image((void*)(intptr_t)texture->getOpenglTextureID(), ImVec2(texture->getWidth(), texture->getHeight()));
    ImGui::End();

    double zero = 0.0f;
    double one = 1.0f;
    double negative_ten = -10.0f;
    double positive_ten = 10.0f;
    ImGui::Begin("Settings");
    ImGui::Text("Sphere 1");
    ImGui::SliderScalarN("Colour##sphere1c", ImGuiDataType_Double, &sphere1.colour.r, 3, &zero, &one, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
    ImGui::SliderScalarN("Position##sphere1p", ImGuiDataType_Double, &sphere1.center.x, 3, &negative_ten, &positive_ten, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
    ImGui::SliderScalar("Radius##sphere1r", ImGuiDataType_Double, &sphere1.radius, &zero, &positive_ten, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
    ImGui::Text("Sphere 2");
    ImGui::SliderScalarN("Colour##sphere2c", ImGuiDataType_Double, &sphere2.colour.r, 3,  &zero, &one, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
    ImGui::SliderScalarN("Position##sphere2p", ImGuiDataType_Double, &sphere2.center.x, 3, &negative_ten, &positive_ten, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
    ImGui::SliderScalar("Radius##sphere2r", ImGuiDataType_Double, &sphere2.radius, &zero, &positive_ten, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
    ImGui::Text("Sphere 3");
    ImGui::SliderScalarN("Colour##sphere3c", ImGuiDataType_Double, &sphere3.colour.r, 3,  &zero, &one, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
    ImGui::SliderScalarN("Position##sphere3p", ImGuiDataType_Double, &sphere3.center.x, 3, &negative_ten, &positive_ten, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
    ImGui::SliderScalar("Radius##sphere3r", ImGuiDataType_Double, &sphere3.radius, &zero, &positive_ten, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
    ImGui::Text("Sphere 4");
    ImGui::SliderScalarN("Colour##sphere4c", ImGuiDataType_Double, &sphere4.colour.r, 3,  &zero, &one, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
    ImGui::SliderScalarN("Position##sphere4p", ImGuiDataType_Double, &sphere4.center.x, 3, &negative_ten, &positive_ten, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
    ImGui::SliderScalar("Radius##sphere4r", ImGuiDataType_Double, &sphere4.radius, &zero, &positive_ten, "%.2f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
    if (ImGui::Button("Update"))
    {
        std::scoped_lock lock(super_cool_mutex2);
        pool.wait_all();

        for (int i = 0; i < update_frame.pixels.size(); ++i)
        {
            update_frame.pixels[i] = {0.0f, 0.0f, 0.0f};
        }

        //raytrace thread will ++ this, so it will be 0 for a fully black frame, which is correct
        current_samples = -1;

        static_cast<core::Sphere&>(*scene.hitables[0]) = sphere1;
        static_cast<core::Sphere&>(*scene.hitables[1]) = sphere2;
        static_cast<core::Sphere&>(*scene.hitables[2]) = sphere3;
        static_cast<core::Sphere&>(*scene.hitables[3]) = sphere4;
    }
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
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    //magic_function();

    {
        texture->update([&](std::vector<core::Pixel>& pixels) {
            std::scoped_lock lock(super_cool_mutex2);
            auto frame_copy = render_frame;
            if (current_samples > 0)
            {
                for (std::size_t i = 0; i < frame_copy.pixels.size(); ++i)
                {
                    frame_copy.pixels[i].r /= static_cast<float>(current_samples);
                    frame_copy.pixels[i].g /= static_cast<float>(current_samples);
                    frame_copy.pixels[i].b /= static_cast<float>(current_samples);
                }
            }
            frame_copy.toPixels(pixels);
        });
    }
    magic_function();

    imgui->render();
    magic_function();

    //todo: seems like window should do this, but then it's GL specific? so renderer?
    SDL_GL_SwapWindow(window_sdl->getRawWindow());
    magic_function();
}

void Game::raytrace_chunk(std::size_t start, std::size_t end)
{
    std::vector<glm::vec3> our_pixels;
    our_pixels.resize(end - start);

    for (std::size_t i = start; i < end; ++i)
    {
        const float rand_u = static_cast<float>(std::rand() % 10000) / 10000.0f;
        const float rand_v = static_cast<float>(std::rand() % 10000) / 10000.0f;

        const double x = i % update_frame.pixels_width;
        const double y = i / update_frame.pixels_width;
        const double u = (x + rand_u) / static_cast<double>(update_frame.pixels_width - 1);
        const double v = 1.0 - ((y + rand_v) / static_cast<double>(update_frame.pixels_height - 1));
        const core::Ray ray{
            camera.origin,
            camera.lower_left_corner + (u * camera.horizontal) + (v * camera.vertical) - camera.origin
        };

        our_pixels[i - start] = cast_ray(ray, scene, 0);;
    }

    {
        std::scoped_lock lock(super_cool_mutex);
        for (std::size_t i = 0; i < our_pixels.size(); ++i)
        {
            update_frame.pixels[i + start] += our_pixels[i];
        }
        percentage += (static_cast<float>(end - start) / static_cast<float>(update_frame.pixels.size())) * 100.0f;
    }
}

void Game::raytrace()
{
    //the higher the max chunks, the more overhead there is with locks, etc. 
    //but if it's too low, then some of the threads could be idling waiting for some expensive chunks to be raytraced
    //so we do a multiple of the thread count, so that by the time it gets to the last "round" of active threads
    // the work should be small enough that the other threads don't idle for very long
    //I could have the other threads start raytracing the next sample if I wanted to, but that makes all of this more complicated
    const auto max_chunks = 10 * pool.thread_count();
    const auto chunk = update_frame.pixels.size() / max_chunks;
    std::vector<std::pair<int, int>> chunks;
    for (int i = 0; i < max_chunks; ++i)
    {
        if (i == max_chunks - 1)
            chunks.push_back({chunk * i, update_frame.pixels.size()});
        else
            chunks.push_back({chunk * i, chunk * (i + 1)});
    }

    std::random_device rd;
    std::mt19937 g(rd());

    while (raytracing)
    {
        //randomise chunk order
        std::shuffle(chunks.begin(), chunks.end(), g);

        //submit chunks
        for (std::size_t i = 0; i < chunks.size(); ++i)
        {
            pool.push_task([this, start = chunks[i].first, end = chunks[i].second]() {
                raytrace_chunk(start, end);
            });
        }

        //wait until all chunks are done, so we know we've finished a complete sample
        pool.wait_all();

        std::scoped_lock lock(super_cool_mutex2);
        render_frame = update_frame;
        current_samples++;
    }
}
