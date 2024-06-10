
// System headers
#include <fmt/core.h>

// Local headers
#include "engine.hpp"
#include "graphics.hpp"
#include "trace.hpp"
#include "input.hpp"
#include "camera.hpp"
#include "shader_manager.hpp"
#include "model_manager.hpp"
#include "object_manager.hpp"
#include "component.hpp"
#include "verlet.hpp"

Engine::Engine() {
}

bool Engine::Initialize() {
    if ( !Graphics::Instance().Initialize() ) {
        Trace::Instance().Message( "Graphics falied to initialize.", FILENAME, LINENUMBER );
        return false;
    }

    if ( !Camera::Instance().Initialize( glm::vec3( 0.f, 5.f, 20.f ) ) ) {
        Trace::Instance().Message( "Camera falied to initialize.", FILENAME, LINENUMBER );
    }

    ShaderManager::Instance().GetShader( "shaders/phong_vertex.glsl",
                                         "shaders/phong_fragment.glsl" );
    ShaderManager::Instance().GetShader( "shaders/instance_vertex.glsl",
                                         "shaders/instance_fragment.glsl" );
    unsigned baseShader = ShaderManager::Instance().GetShader( "shaders/base_vertex.glsl",
                                                               "shaders/base_fragment.glsl" );

    Object* container = ObjectManager::Instance().CreateObject(
        std::vector< Component* >{ new Transform( glm::vec3( 0.f ), glm::vec3( 6 * 1.02f ),
                                                  glm::vec3( 0.f ) ),
                                   ModelManager::Instance().GetModel( "models/sphere.obj",
                                                                      GL_POINTS,
                                                                      baseShader,
                                                                      false ) },
        "Container" );

    VerletManager::Instance().CreateVerlets();
    VerletManager::Instance().SetContainer( container );

    last_time = steady_clock::now();
    accumulator = 0.f;
    time = 0.f;
    is_running = true;

    Trace::Instance().Message( "Engine initialize successful.", FILENAME, LINENUMBER );

    return true;
}

void Engine::Update() {
    while ( is_running ) {
        curr_time = steady_clock::now();
        time_taken = curr_time - last_time;
        delta_time = static_cast< float >( time_taken.count() ) *
                     steady_clock::period::num / steady_clock::period::den;

        last_time = curr_time;
        accumulator += delta_time;

        glfwSetWindowTitle( Graphics::Instance().GetWindow(),
                            fmt::format( "FPS : {:0.2f} | Balls : {:10}", 1.0f / delta_time,
                                         VerletManager::Instance().GetCurrCount() )
                                .c_str() );

        // Non-fixed time step update calls
        Input::Instance().Update();
        // if ( ( 1.f / delta_time ) >= 60.f ) {
        //     VerletManager::AddVerlet();
        // }

        // Fixed time step update calls
        while ( accumulator >= fixed_time_step ) {
            // Call fixed updates here

            VerletManager::Instance().Update();
            // ObjectManager::Instance().FixedUpdate();

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
