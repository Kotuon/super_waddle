
#ifndef VERLET_HPP
#define VERLET_HPP
#pragma once

// std includes
#include <array>
#include <memory>
#include <thread>

// System includes
#include <glm/glm.hpp>

struct Verlet {
    glm::vec3 position;
    glm::vec3 old_position;
    glm::vec3 acceleration;
    static constexpr float radius = 0.15f;
};

class Model;
class Object;

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

    static VerletManager& Instance();

    void SetContainer( Object* Container );

    unsigned GetCurrCount() const;

private:
    void ClearGrid();
    void FillGrid();
    void InsertNode( int x, int y, int z, Verlet* obj );

    void GridCollision();
    void GridCollisionThread( int ThreadId );
    void VerletCollision( Verlet** CurrentCell, Verlet** OtherCell );

    void ContainerCollision();

    static constexpr unsigned MAX = 8000;
    static constexpr int DIM = 58; // static_cast< int >( ( 6 * 1.02f ) / 0.15f ) + 5; // 58;
    static constexpr int CELL_MAX = 4;
    static constexpr unsigned THREAD_COUNT = 24;

    std::array< std::unique_ptr< Verlet >, MAX > verlet_list;
    std::array< float, MAX * 3 > positions{ 0.f };
    std::array< float, MAX > velocities{ 0.f };

    std::array< Verlet*, DIM * DIM * DIM * CELL_MAX > collision_grid{ nullptr };
    std::array< std::thread, THREAD_COUNT > threads;

    Object* container = nullptr;

    Model* model;

    unsigned amount_to_add = 100;

    float dt;

    float add_timer = 0.25f;
    float toggle_timer = 0.25f;
    unsigned curr_count = 0;

    bool force_toggle = false;
};

#endif
