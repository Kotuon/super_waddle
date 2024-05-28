
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

    if ( !Camera::Instance().Initialize( glm::vec3( 0.f, 0.f, 24.f ) ) ) {
        Trace::Instance().Message( "Camera falied to initialize.", FILENAME, LINENUMBER );
    }

    ShaderManager::Instance().GetShader( "shaders/phong_vertex.glsl",
                                         "shaders/phong_fragment.glsl" );
    ShaderManager::Instance().GetShader( "shaders/instance_vertex.glsl",
                                         "shaders/instance_fragment.glsl" );
    unsigned baseShader = ShaderManager::Instance().GetShader( "shaders/base_vertex.glsl",
                                                               "shaders/base_fragment.glsl" );

    Object& container = ObjectManager::Instance().CreateObject(
        std::vector< Component* >{ new Transform,
                                   new Physics,
                                   ModelManager::Instance().GetModel( "models/cube.obj", baseShader,
                                                                      false ) },
        "Container" );


    container.GetComponent< Transform >()->SetPosition( { 0.f, 0.f, 0.f } );
    container.GetComponent< Transform >()->SetScale( glm::vec3( 6 * 2 + 0.15f * 3 ) );
    container.GetComponent< Transform >()->SetRotation( glm::vec3( 0.f ) );

    VerletManager::Instance().CreateVerlets( 1 );

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

        // Non-fixed time step update calls
        Input::Instance().Update();

        // Fixed time step update calls
        while ( accumulator >= fixed_time_step ) {
            // Call fixed updates here

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

Engine& Engine::Instance() {
    static Engine engineInstance;
    return engineInstance;
}
