
// Local headers
#include "engine.hpp"
#include "graphics.hpp"
#include "trace.hpp"
#include "input.hpp"

Engine::Engine() {
}

bool Engine::Initialize() {
    last_time = steady_clock::now();
    accumulator = 0.f;
    time = 0.f;
    is_running = true;

    if ( !Graphics::Instance().Initialize() ) {
        Trace::Instance().Message( "Graphics falied to initialize.", FILENAME, LINENUMBER );
        return false;
    }

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
        Graphics::Instance().Update();

        // Fixed time step update calls
        while ( accumulator >= fixed_time_step ) {
            // Call fixed updates here

            accumulator -= fixed_time_step;
            time += fixed_time_step;
        }
    }
}

void Engine::Shutdown() {
    Graphics::Instance().Shutdown();
}

void Engine::TriggerShutdown() {
    is_running = false;
}

Engine& Engine::Instance() {
    static Engine engineInstance;
    return engineInstance;
}
