
// System headers
#include <GLFW/glfw3.h>

// Local includes
#include "input.hpp"
#include "engine.hpp"
#include "graphics.hpp"

Input::Input() {
}

void Input::Update() {
    GLFWwindow* window = Graphics::Instance().GetWindow();

    // Use escape key for terminating the GLFW window
    if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
        glfwSetWindowShouldClose( window, GL_TRUE );
        Engine::Instance().TriggerShutdown();
    }
}

void Input::CloseWindowCallback( GLFWwindow* Window ) {
    glfwSetWindowShouldClose( Window, GL_TRUE );
    Engine::Instance().TriggerShutdown();
}

Input& Input::Instance() {
    static Input inputInstance;
    return inputInstance;
}
