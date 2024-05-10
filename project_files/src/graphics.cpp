
// std includes
#include <format>

// System headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>

// Local headers
#include "graphics.hpp"
#include "super_waddle/super_waddle.hpp"
#include "trace.hpp"

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
                                            ( const char* )( glGetString( GL_VENDOR ) ),
                                            ( const char* )( glGetString( GL_RENDERER ) ) ),
                               FILENAME, LINENUMBER );

    // Trace::Instance().Message( std::format( "{}: {}",
    //                                         glGetString( GL_VENDOR ),
    //                                         glGetString( GL_RENDERER ) ),
    //                            FILENAME, LINENUMBER );
    // Trace::Instance().Message( "GLFW\t " + std::string( glfwGetVersionString() ),
    //                            FILENAME, LINENUMBER );
    // Trace::Instance().Message( std::format( "OpenGL\t {}", glGetString( GL_VERSION ) ),
    //                            FILENAME, LINENUMBER );
    // Trace::Instance().Message( std::format( "GLSL\t {}",
    //                                         glGetString( GL_SHADING_LANGUAGE_VERSION ) ),
    //                            FILENAME, LINENUMBER );

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

    return true;
}

void Graphics::Update() {
}

void Graphics::Shutdown() {
}

void Graphics::FrameBufferSizeCallback( GLFWwindow*, int Width, int Height ) {
    glViewport( 0, 0, Width, Height );
}

void Graphics::CursorEnterCallback( GLFWwindow*, int Entered ) {
    if ( Entered ) {
        // The cursor entered the content area of the window
        glfwSetInputMode( Graphics::Instance().window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
        // cursorEntered = true;
    } else {
        // The cursor left the content area of the window
    }
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

void runProgram( GLFWwindow* window ) {
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
    glfwSetFramebufferSizeCallback( window, framebuffer_size_callback );
    glfwSetCursorEnterCallback( window, cursor_enter_callback );

    // Set up your scene here (create Vertex Array Objects, etc.)

    // Rendering Loop
    while ( !glfwWindowShouldClose( window ) ) {
        // Clear colour and depth buffers
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        // Draw your scene here

        // Handle other events
        glfwPollEvents();
        handleKeyboardInput( window );

        // Flip buffers
        glfwSwapBuffers( window );
    }
}

void framebuffer_size_callback( GLFWwindow*, int width, int height ) {
    glViewport( 0, 0, width, height );
}

void cursor_enter_callback( GLFWwindow* window, int entered ) {
    if ( entered ) {
        // The cursor entered the content area of the window
        glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
        // cursorEntered = true;
    } else {
        // The cursor left the content area of the window
    }
}

void handleKeyboardInput( GLFWwindow* window ) {
    // Use escape key for terminating the GLFW window
    if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
        glfwSetWindowShouldClose( window, GL_TRUE );
    }
}
