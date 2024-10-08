
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

    UpdateMouse( window );

    // Use escape key for terminating the GLFW window
    if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
        glfwSetWindowShouldClose( window, GL_TRUE );
        Engine::Instance().TriggerShutdown();
    }

    for ( auto key : key_map ) {
        if ( glfwGetKey( window, key.first ) != GLFW_PRESS ) {
            continue;
        }

        for ( auto func : key.second ) {
            func();
        }
    }

    glm::vec3 wasdInput{ 0.f };
    wasdInput.y = glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS ? 1.f : wasdInput.y;
    wasdInput.y = glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS ? -1.f : wasdInput.y;
    wasdInput.x = glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS ? 1.f : wasdInput.x;
    wasdInput.x = glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS ? -1.f : wasdInput.x;

    glm::vec2 arrowInput{ 0.f };
    arrowInput.y = glfwGetKey( window, GLFW_KEY_UP ) ? 1.f : arrowInput.y;
    arrowInput.y = glfwGetKey( window, GLFW_KEY_DOWN ) ? -1.f : arrowInput.y;
    arrowInput.x = glfwGetKey( window, GLFW_KEY_RIGHT ) ? 1.f : arrowInput.x;
    arrowInput.x = glfwGetKey( window, GLFW_KEY_LEFT ) ? -1.f : arrowInput.x;

    for ( auto func : wasd_callbacks ) {
        func( wasdInput );
    }

    for ( auto func : arrow_callbacks ) {
        func( arrowInput );
    }
}

void Input::UpdateMouse( GLFWwindow* Window ) {
    mouse.p_pos = mouse.pos;

    glfwGetCursorPos( Window, &( mouse.pos.x ), &( mouse.pos.y ) );
}

Mouse& Input::GetMouse() {
    return mouse;
}

glm::dvec2 Input::GetMousePosDx() {
    return mouse.pos - mouse.p_pos;
}

void Input::CloseWindowCallback( GLFWwindow* Window ) {
    glfwSetWindowShouldClose( Window, GL_TRUE );
    Engine::Instance().TriggerShutdown();
}

Input& Input::Instance() {
    static Input inputInstance;
    return inputInstance;
}

void Input::AddWASDCallback( std::function< void( glm::vec3 ) > func ) {
    wasd_callbacks.push_back( func );
}

void Input::AddArrowCallback( std::function< void( glm::vec2 ) > func ) {
    arrow_callbacks.push_back( func );
}
