
// std includes
#include <algorithm>

// Local includes
#include "octree.hpp"
#include "vec.hpp"
#include "timer.hpp"

static inline unsigned findex( const unsigned i ) noexcept {
    return i * fvec::VEC4_SIZE;
}

Octree::Octree() {
    for ( unsigned i = 0; i < THREAD_COUNT; ++i ) {
        threads.emplace_back( [this, i]() {
            while ( true ) {
                std::unique_lock< std::mutex > lock( m );
                cv.wait( lock );
                GridCollisionThread( i );
                counter.fetch_add( 1 );
                lock.unlock();
            }
        } );
    }

    for ( unsigned i = 0; i < VerletManager::MAX; ++i ) {
        ids.push_back( i );
    }
}

void Octree::FillTree( const std::array< float, VerletManager::MAX * fvec::VEC4_SIZE >& Pos,
                       const float Radius, const unsigned CurrCount ) noexcept {
    const float diameter = Radius * 2.f;
    const int halfDim = DIM / 2;
    for ( unsigned i = 0; i < CurrCount; ++i ) {
        const int x = std::clamp( static_cast< int >( Pos[findex( i ) + 0] / diameter + halfDim ),
                                  0, DIM - 1 );
        const int y = std::clamp( static_cast< int >( Pos[findex( i ) + 1] / diameter + halfDim ),
                                  0, DIM - 1 );
        const int z = std::clamp( static_cast< int >( Pos[findex( i ) + 2] / diameter + halfDim ),
                                  0, DIM - 1 );

        InsertNode( x, y, z, &ids[i] );
    }
}

void Octree::InsertNode( const int x, const int y, const int z, unsigned* obj ) noexcept {
    int index = ( z + y * DIM + x * DIM * DIM ) * CELL_MAX;
    while ( collision_grid[index] ) {
        index = ( index + 1 ) % ( DIM * DIM * DIM * CELL_MAX );
    }
    collision_grid[index] = obj;
}

void Octree::CheckCollisions() noexcept {
    counter.store( 0 );
    cv.notify_all();
    while ( counter < THREAD_COUNT ) {
    }
}

void Octree::GridCollisionThread( const unsigned ThreadId ) noexcept {
    const unsigned start = 1 + ThreadId * ( ( DIM ) / THREAD_COUNT );
    unsigned end = 1 + ( ThreadId + 1 ) * ( ( DIM ) / THREAD_COUNT );

    if ( ThreadId == THREAD_COUNT - 1 ) {
        end += DIM % THREAD_COUNT - 2;
    }

    for ( unsigned x = start; x < end; ++x ) {
        for ( unsigned y = 1; y < DIM - 1; ++y ) {
            for ( unsigned z = 1; z < DIM - 1; ++z ) {
                unsigned** currentCell = GetNode( x, y, z );

                if ( !currentCell[0] ) {
                    continue;
                }

                for ( int dx = -1; dx <= 1; ++dx ) {
                    for ( int dy = -1; dy <= 1; ++dy ) {
                        for ( int dz = -1; dz <= 1; ++dz ) {
                            unsigned** otherCell = GetNode( x + dx, y + dy, z + dz );

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

void Octree::VerletCollision( unsigned** CurrentCell, unsigned** OtherCell ) noexcept {
    for ( int a = 0; CurrentCell[a]; ++a ) {
        for ( int b = 0; OtherCell[b]; ++b ) {
            verlet_collision_callback( CurrentCell[a], OtherCell[b] );
        }
    }
}
