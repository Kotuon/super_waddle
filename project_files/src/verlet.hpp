
#ifndef VERLET_HPP
#define VERLET_HPP
#pragma once

// std includes
#include <array>

// System includes
#include <glm/glm.hpp>

class Object;

struct VerletManager {
public:
    void CreateVerlets();

    void UpdateVerlets();

    void PhysicsUpdate();

    void DrawVerlets( glm::mat4& Projection );

    static void AddVerlet();
    static VerletManager& Instance();

    void SetContainer( Object* Container );

    unsigned GetCurrCount() const;

private:
    static constexpr unsigned max = 200;

    std::array< Object*, max > verlet_list{ nullptr };
    std::array< float, max * 3 > positions{ 0.f };
    std::array< float, max > velocities{ 0.f };

    Object* container = nullptr;

    unsigned amount_to_add = 10;

    float timer = 1.f;
    unsigned curr_count = 0;
};

#endif
