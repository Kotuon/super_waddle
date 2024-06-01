
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
    void CreateVerlets();

    void UpdateVerlets();


    void DrawVerlets( glm::mat4& Projection );

    static void AddVerlet();
    static VerletManager& Instance();

private:
    std::vector< Object* > verlet_list;
    std::vector< float > positions;
    std::vector< float > velocities;

    float timer = 1.f;
    const unsigned max = 100;
    unsigned curr_count = 0;
};

#endif
