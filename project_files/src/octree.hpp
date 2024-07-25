
#ifndef OCTREE_HPP
#define OCTREE_HPP
#pragma once

// std includes
#include <array>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>
#include <thread>

// Local includes
#include "verlet.hpp"
#include "float_math.hpp"

class Octree {
public:
    Octree();

    template < typename TCallback >
    inline void SetVerletCollisionCallback( TCallback&& Callback ) noexcept {
        verlet_collision_callback = Callback;
    }

    void FillTree( const std::array< float, VerletManager::MAX * fvec::VEC4_SIZE >& Pos,
                   const float Radius, const unsigned CurrCount ) noexcept;
    inline void ClearTree() {
        collision_grid.fill( nullptr );
    }

    inline unsigned** GetNode( const int x, const int y, const int z ) noexcept {
        return &collision_grid[( z + y * DIM + x * DIM * DIM ) * CELL_MAX];
    }

    void CheckCollisions() noexcept;

    void GridCollisionThread( const unsigned ThreadId ) noexcept;
    inline void VerletCollision( unsigned** CurrentCell, unsigned** OtherCell ) noexcept;

private:
    inline void InsertNode( const int x, const int y, const int z, unsigned* obj ) noexcept;

    std::atomic< unsigned > counter = 0;
    std::mutex m;
    std::condition_variable cv;

    static constexpr int DIM = 49;
    static constexpr int CELL_MAX = 4;

    std::function< void( unsigned*, unsigned* ) > verlet_collision_callback;

    unsigned THREAD_COUNT = 4;
    std::vector< std::thread > threads;
    std::vector< unsigned > ids;

    std::array< unsigned*, DIM * DIM * DIM * CELL_MAX > collision_grid{ nullptr };
};

#endif
