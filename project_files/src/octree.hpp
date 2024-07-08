
#ifndef OCTREE_HPP
#define OCTREE_HPP
#pragma once

// std includes
#include <array>
#include <memory>
#include <vector>
#include <thread>

// Local includes
#include "verlet.hpp"

// typedef std::array< std::unique_ptr< Verlet >, VerletManager::MAX > VerletArray;

class Octree {
public:
    template < typename TCallback >
    inline void SetVerletCollisionCallback( TCallback&& Callback ) {
        verlet_collision_callback = Callback;
    }

    void FillTree( std::array< std::unique_ptr< Verlet >, VerletManager::MAX >& Verlets, const float Radius, const unsigned CurrCount );
    void ClearTree();

    Verlet** GetNode( int x, int y, int z ) {
        return &collision_grid[( z + y * DIM + x * DIM * DIM ) * CELL_MAX];
    }

    void CheckCollisions();

    void GridCollisionThread( int ThreadId );
    void VerletCollision( Verlet** CurrentCell, Verlet** OtherCell );

private:
    void InsertNode( int x, int y, int z, Verlet* obj );

    static constexpr int DIM = 58;
    static constexpr int CELL_MAX = 4;
    static constexpr unsigned THREAD_COUNT = 24;

    std::function< void( Verlet*, Verlet* ) > verlet_collision_callback;

    std::array< std::thread, THREAD_COUNT > threads;

    std::array< Verlet*, DIM * DIM * DIM * CELL_MAX > collision_grid{ nullptr };
};

#endif
