
// std includes

// System headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>

// Local headers
#include "graphics.hpp"
#include "super_waddle/super_waddle.hpp"
#include "trace.hpp"
#include "engine.hpp"

static const char* CastToString( const unsigned char* input ) {
    return reinterpret_cast< const char* >( input );
}

Graphics::Graphics() {
}

bool Graphics::Initialize() {
    if ( !glfwInit() ) {
        Trace::Instance().Message( "Could not start GLFW.", FILENAME, LINENUMBER );
        return false;
    }

    // Set core window options (adjust version numbers if needed)
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    // Enable the GLFW runtime error callback function defined previously.
    glfwSetErrorCallback( Graphics::GLFWErrorCallback );

    // Set additional window options
    glfwWindowHint( GLFW_RESIZABLE, windowResizable );
    glfwWindowHint( GLFW_SAMPLES, windowSamples ); // MSAA

    // Create window using GLFW
    window = glfwCreateWindow( windowWidth,
                               windowHeight,
                               windowTitle.c_str(),
                               nullptr,
                               nullptr );

    // Ensure the window is set up correctly
    if ( !window ) {
        Trace::Instance().Message( "Could not open GLFW window.", FILENAME, LINENUMBER );

        glfwTerminate();
        return false;
    }

    // Let the window be the current OpenGL context and initialise glad
    glfwMakeContextCurrent( window );
    gladLoadGL();

    Trace::Instance().Message( fmt::format( "{}: {}",
                                            CastToString( glGetString( GL_VENDOR ) ),
                                            CastToString( glGetString( GL_RENDERER ) ) ),
                               FILENAME, LINENUMBER );
    Trace::Instance().Message( fmt::format( "GLFW\t {}",
                                            glfwGetVersionString() ),
                               FILENAME, LINENUMBER );
    Trace::Instance().Message( fmt::format( "OpenGL\t {}",
                                            CastToString( glGetString( GL_VERSION ) ) ),
                               FILENAME, LINENUMBER );
    Trace::Instance().Message( fmt::format( "GLSL\t {}",
                                            CastToString( glGetString( GL_SHADING_LANGUAGE_VERSION ) ) ),
                               FILENAME, LINENUMBER );

    // Enable depth (Z) buffer (accept "closest" fragment)
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    // Configure miscellaneous OpenGL settings
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glPointSize( 3.0 );

    // Set default colour after clearing the colour buffer
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClearStencil( 0 );

    // Set callbacks
    glfwSetFramebufferSizeCallback( window, Graphics::FrameBufferSizeCallback );
    glfwSetCursorEnterCallback( window, Graphics::CursorEnterCallback );
    glfwSetWindowCloseCallback( window, Graphics::CloseWindowCallback );

    return true;
}

void Graphics::Update() {
    // Clear colour and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Draw your scene here

    // Handle other events
    glfwPollEvents();
    handleKeyboardInput( window );

    // Flip buffers
    glfwSwapBuffers( window );
}

void Graphics::Shutdown() {
    // Terminate GLFW (no need to call glfwDestroyWindow)
    glfwTerminate();
}

void Graphics::FrameBufferSizeCallback( GLFWwindow*, int Width, int Height ) {
    glViewport( 0, 0, Width, Height );
}

void Graphics::CursorEnterCallback( GLFWwindow*, int Entered ) {
    if ( Entered ) {
        // The cursor entered the content area of the window
        // glfwSetInputMode( Graphics::Instance().window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
        // cursorEntered = true;
    } else {
        // The cursor left the content area of the window
    }
}

void Graphics::CloseWindowCallback( GLFWwindow* Window ) {
    glfwSetWindowShouldClose( Window, GL_TRUE );
    Engine::Instance().TriggerShutdown();
}

void Graphics::GLFWErrorCallback( int Error, const char* Description ) {
    std::string message = "GLFW returned an error: " +
                          std::string( Description ) +
                          std::to_string( Error );

    Trace::Instance().Message( message, FILENAME, LINENUMBER );
}

Graphics& Graphics::Instance() {
    static Graphics graphicsInstance;
    return graphicsInstance;
}

void handleKeyboardInput( GLFWwindow* window ) {
    // Use escape key for terminating the GLFW window
    if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
        glfwSetWindowShouldClose( window, GL_TRUE );
        Engine::Instance().TriggerShutdown();
    }
}
