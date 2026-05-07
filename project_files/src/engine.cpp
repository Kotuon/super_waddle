
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
#include "editor.hpp"

Engine::Engine() {
}

bool Engine::Initialize() {
    if ( !Graphics::Instance().Initialize() ) {
        Trace::message( "Graphics falied to initialize." );
        return false;
    }

    if ( !Camera::Instance().Initialize( glm::vec3( 0.f, 5.f, 20.f ) ) ) {
        Trace::message( "Camera falied to initialize." );
    }

    if ( !Editor::Instance().Initialize() ) {
        Trace::message( "Editor failed to initialize." );
    }

    ShaderManager::Instance().GetShader( "shaders/phong_vertex.glsl",
                                         "shaders/phong_fragment.glsl" );
    ShaderManager::Instance().GetShader( "shaders/instance_vertex.glsl",
                                         "shaders/instance_fragment.glsl" );
    ShaderManager::Instance().GetShader( "shaders/base_vertex.glsl",
                                         "shaders/base_fragment.glsl" );

    VerletManager::Instance().CreateVerlets( ContainerShape::Sphere );

    last_time = steady_clock::now();
    accumulator = 0.f;
    time = 0.f;
    is_running = true;

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

            for ( auto& func : fixed_update_callbacks ) {
                func();
            }

            accumulator -= fixed_time_step;
            time += fixed_time_step;
        }

        // Non-fixed time step update calls
        // TODO: will be moved around

        for ( auto& func : update_callbacks ) {
            func();
        }

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
