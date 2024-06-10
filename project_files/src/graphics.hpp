
#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP
#pragma once

// System headers
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>
#include <memory>
#include <glm/glm.hpp>

class Object;

class Graphics {
public:
    bool Initialize();
    void Update();
    void Shutdown();

    GLFWwindow* GetWindow() const;

    static void FrameBufferSizeCallback( GLFWwindow* Window, int Width, int Height );
    static void CursorEnterCallback( GLFWwindow* Window, int Entered );

    static void GLFWErrorCallback( int Error, const char* Description );
    static Graphics& Instance();

private:
    Graphics();

    void DrawNormal( Object* ObjectToDraw, glm::mat4& Projection );
    void DrawInstaned( Object* ObjectToDraw, glm::mat4& Projection );

    GLFWwindow* window;

    glm::mat4 projection;
};

// Function for handling keypresses
void handleKeyboardInput( GLFWwindow* Window );

#endif
