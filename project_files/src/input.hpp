
#ifndef INPUT_HPP
#define INPUT_HPP
#pragma once

// std includes
#include <vector>
#include <unordered_map>

// System includes
#include <glm/glm.hpp>

struct GLFWwindow;

struct Mouse {
    glm::dvec2 pos{ 0.0, 0.0 };
    glm::dvec2 p_pos{ 0.0, 0.0 };
};

class Input {
public:
    void Update();

    Mouse& GetMouse();
    glm::dvec2 GetMousePosDx();

    static void CloseWindowCallback( GLFWwindow* Window );

    static Input& Instance();

    void AddWASDCallback( void ( *func )( glm::vec3 ) );
    void AddArrowCallback( void ( *func )( glm::vec2 ) );

    void AddCallback( int key, void ( *func )() );

private:
    Input();

    void UpdateMouse( GLFWwindow* Window );

    std::vector< void ( * )( glm::vec3 ) > wasd_callbacks;
    std::vector< void ( * )( glm::vec2 ) > arrow_callbacks;

    std::unordered_map< int, std::vector< void ( * )() > > key_map;

    Mouse mouse;
};

#endif
