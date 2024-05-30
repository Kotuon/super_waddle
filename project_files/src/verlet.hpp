
#ifndef VERLET_HPP
#define VERLET_HPP
#pragma once

// std includes
#include <vector>

// System includes
#include <glm/glm.hpp>

class Object;

struct VerletManager {
public:
    void CreateVerlets( int Amount );

    void UpdateVerlets();

    void DrawVerlets( glm::mat4& Projection );

    static VerletManager& Instance();

private:
    std::vector< Object* > verlet_list;
    std::vector< float > positions;
    std::vector< float > velocities;
};

#endif
