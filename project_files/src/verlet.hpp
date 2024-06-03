
#ifndef VERLET_HPP
#define VERLET_HPP
#pragma once

// std includes
#include <array>
#include <memory>

// System includes
#include <glm/glm.hpp>

struct Verlet {
    glm::vec3 position;
    glm::vec3 old_position;
    glm::vec3 acceleration;
    float radius;
};

class Model;
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
    static constexpr unsigned max = 2000;

    std::array< std::unique_ptr< Verlet >, max > verlet_list;
    std::array< float, max * 3 > positions{ 0.f };
    std::array< float, max > velocities{ 0.f };

    Object* container = nullptr;

    Model* model;

    unsigned instance_shader;

    unsigned amount_to_add = 10;

    float timer = 0.25f;
    unsigned curr_count = 0;
};

#endif
