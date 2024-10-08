
// Standard headers
#include <cstdlib>
#include <windows.h>

// System headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>

// Local headers
#include "super_waddle/super_waddle.hpp"
#include "graphics.hpp"
#include "engine.hpp"
#include "trace.hpp"
#include "crash_handler.hpp"

int main( int, char*[] ) {
    SetupDump();

    char dir[256];
    GetModuleFileName( nullptr, dir, 256 );
    Trace::Message( fmt::format( "{}", dir ) );

    // Initialize application
    bool result = Engine::Instance().Initialize();
    if ( !result ) {
        return EXIT_FAILURE;
    }

    // Running application
    Engine::Instance().Update();

    // Shutdown application
    Engine::Instance().Shutdown();

    return EXIT_SUCCESS;
}
