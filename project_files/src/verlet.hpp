
#ifndef VERLET_HPP
#define VERLET_HPP
#pragma once

// std includes
#include <array>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

// System includes
#include <glm/glm.hpp>

// Local includes
#include "float_math.hpp"

class KDTree;
class Octree;
class Model;

enum ContainerShape {
    Sphere,
    Cube,
};

struct Container {
    Model* model;
    glm::mat4 matrix;
    float model_radius;
    float collision_radius = 6.f;
    ContainerShape shape;
};

struct VerletManager {
public:
    void CreateVerlets( const ContainerShape CShape );

    void Update();
    void CollisionUpdate();
    void PositionUpdate() noexcept;
    void PositionUpdateThread( const int ThreadId ) noexcept;

    void DrawVerlets();

    void AddVerlet();
    void RemoveVerlet();
    void ApplyForce();
    void ToggleForce();

    static VerletManager& Instance();

    unsigned GetCurrCount() const;

    void DisplayMenu();

    static constexpr unsigned MAX = 80000;

private:
    void SetupVerletPosition( const unsigned i );
    void SetupVerlets();
    void SetupContainer( const ContainerShape CShape );

    void CheckCollisionBetweenVerlets( const unsigned* Verlet1, const unsigned* Verlet2 ) noexcept;

    void ContainerCollision();
    inline void SphereCollision( const unsigned* verlet ) noexcept;

    std::array< float, MAX * fvec::VEC4_SIZE > f_pos{ 0.f };
    std::array< float, MAX * fvec::VEC4_SIZE > f_oldpos{ 0.f };
    std::array< float, MAX * fvec::VEC4_SIZE > f_accel{ 0.f };
    std::array< float, MAX > velocities{ 0.f };

    glm::mat4 projection;

    Model* model;
    Container container;

    float force_position[fvec::VEC4_SIZE]{ 0.f, 4.f, 0.f, 0.f };
    float grav_vec[fvec::VEC4_SIZE]{ 0.f, -4.5f, 0.f, 0.f };

    int THREAD_COUNT = 24;
    std::vector< std::thread > threads;

    std::unique_ptr< Octree > octree;

    float vel_damping = 20.f;

    float verlet_radius = 0.15f;
    float verlet_diameter = verlet_radius * 2.f;

    float dt;

    float add_timer = 0.05f;
    float add_cooldown = 0.1f;

    float toggle_timer = 0.25f;

    float fps_limit = 90.f;

    int amount_to_add = 100;
    unsigned curr_count = 0;

    bool should_simulate = true;

    bool force_toggle = false;
};

#endif
