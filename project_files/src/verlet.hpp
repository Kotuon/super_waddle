
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

    void CollisionUpdate();

    void PositionUpdate();

    void DrawVerlets( glm::mat4& Projection );

    static void AddVerlet();
    static VerletManager& Instance();

    void SetContainer( Object* Container );

    unsigned GetCurrCount() const;

private:
    void ClearGrid();
    void FillGrid();
    void InsertNode( int x, int y, int z, Verlet* obj );

    void GridCollision( Verlet** CurrentCell, Verlet** OtherCell );
    void VerletCollision( Verlet* a, Verlet* b );

    static constexpr unsigned MAX = 2000;
    static constexpr int DIM = static_cast< int >( ( 6 * 1.02f ) / 0.15f ) + 5; // 58;
    static constexpr int CELL_MAX = 32;

    std::array< std::unique_ptr< Verlet >, MAX > verlet_list;
    std::array< float, MAX * 3 > positions{ 0.f };
    std::array< float, MAX > velocities{ 0.f };

    // std::array< Verlet*, DIM * DIM * DIM * CELL_MAX > collision_grid{ nullptr };
    std::array< std::array< std::array< std::array< Verlet*,
                                                    CELL_MAX >,
                                        DIM >,
                            DIM >,
                DIM >
        collision_grid{ nullptr };

    Object* container = nullptr;

    Model* model;

    unsigned instance_shader;

    unsigned amount_to_add = 10;

    float dt;

    float timer = 0.25f;
    unsigned curr_count = 0;
};

#endif
