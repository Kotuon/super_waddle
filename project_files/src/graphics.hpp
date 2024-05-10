
#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP
#pragma once

// System headers
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>
#include <memory>

class Graphics {
public:
    bool Initialize();
    void Update();
    void Shutdown();

    static void FrameBufferSizeCallback( GLFWwindow* Window, int Width, int Height );
    static void CursorEnterCallback( GLFWwindow* Window, int Entered );

    static void GLFWErrorCallback( int Error, const char* Description );
    static Graphics& Instance();

private:
    GLFWwindow* window;
};

// Main OpenGL program
void runProgram( GLFWwindow* Window );

// Function for handling keypresses
void handleKeyboardInput( GLFWwindow* Window );

void framebuffer_size_callback( GLFWwindow* window, int width, int height );
void cursor_enter_callback( GLFWwindow* window, int entered );

// Checks for whether an OpenGL error occurred. If one did,
// it prints out the error type and ID
inline void printGLError() {
    int errorID = glGetError();

    if ( errorID != GL_NO_ERROR ) {
        std::string errorString;

        switch ( errorID ) {
        case GL_INVALID_ENUM:
            errorString = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_OPERATION:
            errorString = "GL_INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            errorString = "GL_OUT_OF_MEMORY";
            break;
        case GL_STACK_UNDERFLOW:
            errorString = "GL_STACK_UNDERFLOW";
            break;
        case GL_STACK_OVERFLOW:
            errorString = "GL_STACK_OVERFLOW";
            break;
        default:
            errorString = "[Unknown error ID]";
            break;
        }

        fprintf( stderr, "An OpenGL error occurred (%i): %s.\n",
                 errorID, errorString.c_str() );
    }
}

#endif
