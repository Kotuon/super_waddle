
#ifndef VERLET_HPP
#define VERLET_HPP
#pragma once

// std includes
#include <array>
#include <memory>
#include <thread>
#include <queue>

// System includes
#include <glm/glm.hpp>

// Local includes
#include "math.hpp"

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
    float position[VEC3];
    float old_position[VEC3];
    float acceleration[VEC3];
};

struct VerletManager {
public:
    void CreateVerlets( ContainerShape CShape );

    void Update();
    void CollisionUpdate();
    void PositionUpdate();

    void DrawVerlets();

    void AddVerlet();
    void RemoveVerlet();
    void ApplyForce();
    void ToggleForce();

    static VerletManager& Instance();

    unsigned GetCurrCount() const;

    void DisplayMenu();

private:
    void ClearGrid();
    void FillGrid();
    void InsertNode( int x, int y, int z, Verlet* obj );

    void SetupVerletPosition( Verlet* verlet, int i );
    void SetupVerlets();
    void SetupContainer( ContainerShape CShape );

    void GridCollision();
    void GridCollisionThread( int ThreadId );
    void VerletCollision( Verlet** CurrentCell, Verlet** OtherCell );

    void ContainerCollision();

    static constexpr unsigned MAX = 40000;
    static constexpr int DIM = 58;
    static constexpr int CELL_MAX = 4;
    static constexpr unsigned THREAD_COUNT = 24;

    std::array< std::unique_ptr< Verlet >, MAX > verlet_list{ nullptr };
    std::array< float, MAX * 3 > positions{ 0.f };
    std::array< float, MAX > velocities{ 0.f };

    std::array< Verlet*, DIM * DIM * DIM * CELL_MAX > collision_grid{ nullptr };
    std::array< std::thread, THREAD_COUNT > threads;

    glm::mat4 projection;

    Model* model;
    Container container;

    float force_vec[VEC3] = { 0.f, 3.f, 0.f };
    float grav_vec[VEC3] = { 0.f, -4.5f, 0.f };
    float vel_damping = 20.f;

    float verlet_radius = 0.15f;

    float dt;

    float add_timer = 0.25f;
    float add_cooldown = 0.1f;

    float toggle_timer = 0.25f;

    int amount_to_add = 100;
    unsigned curr_count = 0;

    bool should_simulate = true;

    bool force_toggle = false;
};

#endif
