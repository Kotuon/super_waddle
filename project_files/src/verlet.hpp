
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
#include "math.hpp"

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

struct Verlet {
    vec4 position{ 0.f };
    vec4 old_position{ 0.f };
    vec4 acceleration{ 0.f };
};

struct VerletManager {
public:
    void CreateVerlets( ContainerShape CShape );

    void Update();
    void CollisionUpdate();
    void PositionUpdate() noexcept;
    void PositionUpdateThread( int ThreadId ) noexcept;

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
    void SetupVerletPosition( Verlet* verlet, int i );
    void SetupVerlets();
    void SetupContainer( ContainerShape CShape );

    void CheckCollisionBetweenVerlets( Verlet* Verlet1, Verlet* Verlet2 );

    void CheckCollisionsWithKDTree( int ThreadId );

    void ContainerCollision();

    std::array< std::unique_ptr< Verlet >, MAX > verlet_list{ nullptr };
    std::array< float, MAX * 3 > positions{ 0.f };
    std::array< float, MAX > velocities{ 0.f };

    glm::mat4 projection;

    Model* model;
    Container container;

    vec4 force_position{ 0.f, 4.f, 0.f, 0.f };
    vec4 grav_vec{ 0.f, -4.5f, 0.f, 0.f };

    int THREAD_COUNT = 24;
    std::vector< std::thread > threads;

    std::unique_ptr< KDTree > kdtree;
    std::unique_ptr< Octree > octree;

    float vel_damping = 20.f;

    float verlet_radius = 0.15f;

    float dt;

    float add_timer = 0.25f;
    float add_cooldown = 0.1f;

    float toggle_timer = 0.25f;

    float fps_limit = 90.f;

    int amount_to_add = 100;
    unsigned curr_count = 0;

    bool should_simulate = true;

    bool force_toggle = false;
};

#endif
