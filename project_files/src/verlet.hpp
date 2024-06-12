
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

struct Verlet {
    float position[VEC3];
    float old_position[VEC3];
    float acceleration[VEC3];
    float radius = 0.15f;
};

class Model;

struct VerletManager {
public:
    void CreateVerlets();

    void Update();
    void CollisionUpdate();
    void PositionUpdate();

    void DrawVerlets( glm::mat4& Projection );

    static void AddVerlet();
    static void ApplyForce();
    static void ToggleForce();

    void SetContainerRadius( float Radius );

    static VerletManager& Instance();

    unsigned GetCurrCount() const;

private:
    void ClearGrid();
    void FillGrid();
    void InsertNode( int x, int y, int z, Verlet* obj );

    void GridCollision();
    void GridCollisionThread( int ThreadId );
    void VerletCollision( Verlet** CurrentCell, Verlet** OtherCell );

    void ContainerCollision();

    static constexpr unsigned MAX = 20000;
    static constexpr int DIM = 58;
    static constexpr int CELL_MAX = 4;
    static constexpr unsigned THREAD_COUNT = 24;

    std::array< std::unique_ptr< Verlet >, MAX > verlet_list;
    std::array< float, MAX * 3 > positions{ 0.f };
    std::array< float, MAX > velocities{ 0.f };
    std::array< float, MAX > scales{ 0.f };

    std::array< Verlet*, DIM * DIM * DIM * CELL_MAX > collision_grid{ nullptr };
    std::array< std::thread, THREAD_COUNT > threads;

    Model* model;

    static constexpr float force_vec[VEC3] = { 0.f, 3.f, 0.f };
    static constexpr float grav_vec[VEC3] = { 0.f, -4.5f, 0.f };

    unsigned amount_to_add = 100;

    float c_radius;

    float dt;

    float add_timer = 0.25f;
    float toggle_timer = 0.25f;
    unsigned curr_count = 10000;

    bool force_toggle = false;
};

#endif
