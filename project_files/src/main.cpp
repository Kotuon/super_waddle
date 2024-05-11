// Local headers
#include "super_waddle/super_waddle.hpp"
#include "graphics.hpp"
#include "engine.hpp"

// System headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard headers
#include <cstdlib>

int main( int, char*[] ) {
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
