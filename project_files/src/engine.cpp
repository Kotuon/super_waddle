
// System headers
#include <fmt/core.h>

// Local headers
#include "profiler.hpp"
#include "engine.hpp"
#include "graphics.hpp"
#include "trace.hpp"
#include "input.hpp"
#include "camera.hpp"
#include "shader_manager.hpp"
#include "model_manager.hpp"
#include "verlet.hpp"

static float ContainerRadius = 6.f * 1.02f;
// static float ContainerRadius = 6.f * 2.f + 0.15f * 3.f;

Engine::Engine() {
}

bool Engine::Initialize() {
    if ( !Graphics::Instance().Initialize() ) {
        Trace::Message( "Graphics falied to initialize.", FILENAME, LINENUMBER );
        return false;
    }

    if ( !Camera::Instance().Initialize( glm::vec3( 0.f, 5.f, 20.f ) ) ) {
        Trace::Message( "Camera falied to initialize.", FILENAME, LINENUMBER );
    }

    ShaderManager::Instance().GetShader( "shaders/phong_vertex.glsl",
                                         "shaders/phong_fragment.glsl" );
    ShaderManager::Instance().GetShader( "shaders/instance_vertex.glsl",
                                         "shaders/instance_fragment.glsl" );
    unsigned base_shader = ShaderManager::Instance().GetShader( "shaders/base_vertex.glsl",
                                                                "shaders/base_fragment.glsl" );

    Model* base_model = ModelManager::Instance().GetModel( "models/sphere.obj", GL_POINTS,
                                                           base_shader, false );

    // Model* base_model = ModelManager::Instance().GetModel( "models/cube.obj", GL_TRIANGLES,
    //                                                        base_shader, false );

    Graphics::Instance().SetContainer( base_model, ContainerRadius );

    VerletManager::Instance().CreateVerlets( ContainerShape::Sphere );
    VerletManager::Instance().SetContainerRadius( 6.f );

    last_time = steady_clock::now();
    accumulator = 0.f;
    time = 0.f;
    is_running = true;

    Trace::Message( "Engine initialize successful.", FILENAME, LINENUMBER );

    return true;
}

void Engine::Update() {
    Profiler profiler;

    while ( is_running ) {
        curr_time = steady_clock::now();
        time_taken = curr_time - last_time;
        delta_time = static_cast< float >( time_taken.count() ) *
                     steady_clock::period::num / steady_clock::period::den;

        last_time = curr_time;
        accumulator += delta_time;

        glfwSetWindowTitle( Graphics::Instance().GetWindow(),
                            fmt::format( "FPS : {:0.2f} | Balls : {:10} | Time : {:5.2f}",
                                         1.0f / delta_time,
                                         VerletManager::Instance().GetCurrCount(), time )
                                .c_str() );

        // Non-fixed time step update calls
        Input::Instance().Update();

        // Fixed time step update calls
        while ( accumulator >= fixed_time_step ) {
            // Call fixed updates here

            VerletManager::Instance().Update();

            accumulator -= fixed_time_step;
            time += fixed_time_step;
        }

        // Non-fixed time step update calls
        // TODO: will be moved around
        Camera::Instance().Update();

        Graphics::Instance().Update();
    }
}

void Engine::Shutdown() {
    Graphics::Instance().Shutdown();
}

void Engine::TriggerShutdown() {
    is_running = false;
}

float Engine::GetDeltaTime() const {
    return delta_time;
}

float Engine::GetTotalTime() const {
    return time;
}

float Engine::GetFixedTimeStep() const {
    return fixed_time_step;
}

Engine& Engine::Instance() {
    static Engine engineInstance;
    return engineInstance;
}
