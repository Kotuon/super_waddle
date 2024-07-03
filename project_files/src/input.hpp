
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

    void AddWASDCallback( std::function< void( glm::vec3 ) > );
    void AddArrowCallback( std::function< void( glm::vec2 ) > );

    template < typename TCallback >
    inline void AddCallback( int key, TCallback&& Callback ) {
        key_map[key].push_back( Callback );
    }

private:
    Input();

    void UpdateMouse( GLFWwindow* Window );

    std::vector< std::function< void( glm::vec3 ) > > wasd_callbacks;
    std::vector< std::function< void( glm::vec2 ) > > arrow_callbacks;

    std::unordered_map< int, std::vector< std::function< void() > > > key_map;

    Mouse mouse;
};

#endif
