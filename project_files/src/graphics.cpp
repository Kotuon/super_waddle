
// Local headers
#include "graphics.hpp"
#include "super_waddle/super_waddle.hpp"

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
        //cursorEntered = true;
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
