
#ifndef KDTREE_HPP
#define KDTREE_HPP
#pragma once

// std includes
#include <vector>
#include <memory>
#include <deque>

// Local includes
#include "math.hpp"

#define BANK_SIZE 80000

struct Verlet;

struct Node {
    Node() : axis( -1 ), idx( -1 ), left_child( nullptr ), right_child( nullptr ) {}

    int axis;
    int idx;
    Node* left_child;
    Node* right_child;
};

class MemoryBank {
public:
    MemoryBank() : bank( BANK_SIZE ) {
        for ( int i = 0; i < BANK_SIZE; ++i ) {
            bank[i] = reinterpret_cast< Node* >( new char[sizeof( Node )] );
        }
    }

    ~MemoryBank() {
        int c = 0;
        for ( Node* to_delete : bank ) {
            if ( ( BANK_SIZE - counter ) <= c ) {
                to_delete->~Node();
            }
            ++c;
            delete[] reinterpret_cast< char* >( to_delete );
        }
    }

    void Allocate() {
        for ( int i = 0; i < BANK_SIZE; ++i ) {
            bank.push_back( reinterpret_cast< Node* >( new char[sizeof( Node )] ) );
        }
    }

    Node* Get() {
        if ( bank.empty() ) {
            Allocate();
        }

        Node* front = bank.front();
        bank.pop_front();
        if ( counter >= BANK_SIZE ) {
            front->~Node();
        } else {
            ++counter;
        }
        return front;
    }

    void Store( Node* to_add ) {
        bank.push_back( to_add );
    }

private:
    std::deque< Node* > bank;
    int counter = 0;
};

class KDTree {
public:
    KDTree();
    KDTree( std::unique_ptr< Verlet >* PointsList, unsigned Size );
    ~KDTree();

    void BuildTree( unsigned Size );
    void ClearTree();

    std::vector< int > SphereSearchTree( vec4 SearchOrigin, float Radius );

private:
    Node* BuildNode( int* Indices, int PointsSize, int Depth );
    void ClearNode( Node* NodeToDelete );

    void SphereSearchNode( const Node* StartNode, vec4 SearchOrigin, float Radius,
                           std::vector< int >& Targets );

    unsigned points_size;
    std::unique_ptr< Verlet >* points;
    Node* head;

    MemoryBank memory_bank;
};

#endif
