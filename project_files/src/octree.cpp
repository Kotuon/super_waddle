
// std includes
#include <algorithm>

// Local includes
#include "octree.hpp"
#include "math.hpp"

Octree::Octree() {
    THREAD_COUNT = std::thread::hardware_concurrency();
    for ( int i = 0; i < THREAD_COUNT; ++i ) {
        threads.emplace_back();
    }
}

void Octree::FillTree( std::array< std::unique_ptr< Verlet >, VerletManager::MAX >& Verlets, float Radius, unsigned CurrCount ) {
    for ( unsigned i = 0; i < CurrCount; ++i ) {
        Verlet* verlet = Verlets[i].get();

        int x = static_cast< int >( verlet->position[0] / ( Radius * 2 ) + DIM / 2 );
        int y = static_cast< int >( verlet->position[1] / ( Radius * 2 ) + DIM / 2 );
        int z = static_cast< int >( verlet->position[2] / ( Radius * 2 ) + DIM / 2 );

        x = std::clamp< int >( x, 0, DIM - 1 );
        y = std::clamp< int >( y, 0, DIM - 1 );
        z = std::clamp< int >( z, 0, DIM - 1 );

        InsertNode( x, y, z, verlet );
    }
}

void Octree::ClearTree() {
    collision_grid.fill( nullptr );
}

void Octree::InsertNode( int x, int y, int z, Verlet* obj ) {
    Verlet** currentCell = &collision_grid[( z + y * DIM + x * DIM * DIM ) * CELL_MAX];

    int i = 0;
    while ( currentCell[i] ) {
        i += 1;
    }
    collision_grid[i + ( z + y * DIM + x * DIM * DIM ) * CELL_MAX] = obj;
}

void Octree::CheckCollisions() {
    for ( int i = 0; i < THREAD_COUNT; ++i ) {
        threads[i] = std::thread( &Octree::GridCollisionThread, this, i );
    }
    for ( std::thread& thd : threads ) {
        thd.join();
    }
}

void Octree::GridCollisionThread( int ThreadId ) {
    unsigned start = 1 + ThreadId * ( ( DIM ) / THREAD_COUNT );
    unsigned end = 1 + ( ThreadId + 1 ) * ( ( DIM ) / THREAD_COUNT );

    if ( ThreadId == THREAD_COUNT - 1 ) {
        end += DIM % THREAD_COUNT - 2;
    }

    for ( unsigned x = start; x < end; ++x ) {
        for ( unsigned y = 1; y < DIM - 1; ++y ) {
            for ( unsigned z = 1; z < DIM - 1; ++z ) {
                Verlet** currentCell = GetNode( x, y, z );

                if ( !currentCell[0] ) {
                    continue;
                }

                for ( int dx = -1; dx <= 1; ++dx ) {
                    for ( int dy = -1; dy <= 1; ++dy ) {
                        for ( int dz = -1; dz <= 1; ++dz ) {
                            Verlet** otherCell = GetNode( x + dx, y + dy, z + dz );

                            if ( !otherCell[0] ) {
                                continue;
                            }
                            VerletCollision( currentCell, otherCell );
                        }
                    }
                }
            }
        }
    }
}

void Octree::VerletCollision( Verlet** CurrentCell, Verlet** OtherCell ) {
    for ( int a = 0; CurrentCell[a]; ++a ) {
        for ( int b = 0; OtherCell[b]; ++b ) {
            verlet_collision_callback( CurrentCell[a], OtherCell[b] );
        }
    }
}
