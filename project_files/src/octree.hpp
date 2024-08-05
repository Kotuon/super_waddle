
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
    Octree();

    template < typename TCallback >
    inline void SetVerletCollisionCallback( TCallback&& Callback ) noexcept {
        verlet_collision_callback = Callback;
    }

    void FillTree( std::array< std::unique_ptr< Verlet >, VerletManager::MAX >& Verlets,
                   const float Radius, const unsigned CurrCount ) noexcept;
    inline void ClearTree() {
        collision_grid.fill( nullptr );
    }

    inline Verlet** GetNode( int x, int y, int z ) {
        return &collision_grid[( z + y * DIM + x * DIM * DIM ) * CELL_MAX];
    }

    void CheckCollisions() noexcept;

    void GridCollisionThread( int ThreadId ) noexcept;
    inline void VerletCollision( Verlet** CurrentCell, Verlet** OtherCell ) noexcept;

private:
    inline void InsertNode( int x, int y, int z, Verlet* obj ) noexcept;

    static constexpr int DIM = 58;
    static constexpr int CELL_MAX = 4;

    std::function< void( Verlet*, Verlet* ) > verlet_collision_callback;

    int THREAD_COUNT = 24;
    std::vector< std::thread > threads;

    std::array< Verlet*, DIM * DIM * DIM * CELL_MAX > collision_grid{ nullptr };
};

#endif
