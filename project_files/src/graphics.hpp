
#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP
#pragma once

// std includes
#include <functional>
#include <vector>

// System headers
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>
#include <memory>
#include <glm/glm.hpp>

class Model;

class Graphics {
public:
    bool Initialize();
    void Update();
    void Shutdown();

    void DrawNormal( Model* Model, glm::mat4& Matrix );

    GLFWwindow* GetWindow() const;

    static void FrameBufferSizeCallback( GLFWwindow* Window, int Width, int Height );
    static void CursorEnterCallback( GLFWwindow* Window, int Entered );

    static void GLFWErrorCallback( int Error, const char* Description );

    template < typename TCallback >
    inline void AddRenderCallback( TCallback&& Callback ) {
        render_callbacks.push_back( Callback );
    }

    glm::mat4 GetProjection();

    static Graphics& Instance();

private:
    Graphics();

    std::vector< std::function< void() > > render_callbacks;

    GLFWwindow* window;

    glm::mat4 projection;
};

// Function for handling keypresses
void handleKeyboardInput( GLFWwindow* Window );

#endif
