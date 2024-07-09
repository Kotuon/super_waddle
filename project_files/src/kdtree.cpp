
// std includes
#include <algorithm>
#include <numeric>
#include <vector>

// Local includes
#include "kdtree.hpp"
#include "verlet.hpp"

KDTree::KDTree() : head( nullptr ), memory_bank() {}

KDTree::KDTree( std::unique_ptr< Verlet >* PointsList,
                unsigned Size ) : head( nullptr ), points( PointsList ),
                                  points_size( Size ), memory_bank() {}

KDTree::~KDTree() {
    ClearTree();
}

void KDTree::BuildTree( unsigned Size ) {
    points_size = Size;
    std::vector< int > indices( points_size );
    std::iota( indices.begin(), indices.end(), 0 );

    head = BuildNode( indices.data(), points_size, 0 );
}

void KDTree::ClearTree() {
    ClearNode( head );
    head = nullptr;
}

std::vector< int > KDTree::SphereSearchTree( vec4 SearchOrigin, float Radius ) {
    std::vector< int > targets;

    SphereSearchNode( head, SearchOrigin, Radius, targets );

    return targets;
}

Node* KDTree::BuildNode( int* Indices, int PointsSize, int Depth ) {
    if ( PointsSize <= 0 ) {
        return nullptr;
    }

    const int axis = Depth % 3;

    std::sort( Indices, Indices + PointsSize, [&]( const int idx1, const int idx2 ) {
        return ( points[idx1] )->position[axis] < ( points[idx2] )->position[axis];
    } );

    const int mid = ( PointsSize - 1 ) / 2;

    // Node* node = new Node();
    Node* node = memory_bank.Get();
    node->axis = axis;
    node->idx = Indices[mid];
    node->left_child = BuildNode( Indices, mid, Depth + 1 );
    node->right_child = BuildNode( Indices + mid + 1, PointsSize - mid - 1, Depth + 1 );

    return node;
}

void KDTree::ClearNode( Node* NodeToDelete ) {
    if ( !NodeToDelete ) {
        return;
    }

    if ( NodeToDelete->left_child ) {
        ClearNode( NodeToDelete->left_child );
    }

    if ( NodeToDelete->right_child ) {
        ClearNode( NodeToDelete->right_child );
    }

    memory_bank.Store( NodeToDelete );
}

void KDTree::SphereSearchNode( const Node* StartNode, vec4 SearchOrigin, float Radius,
                               std::vector< int >& Targets ) {
    if ( !StartNode ) {
        return;
    }

    const vec4 median = ( points[StartNode->idx] )->position;

    const float distSquared = vec_distance_squared( SearchOrigin, median );
    if ( distSquared < Radius * Radius ) {
        Targets.push_back( StartNode->idx );
    }

    if ( SearchOrigin[StartNode->axis] < median[StartNode->axis] ) {
        SphereSearchNode( StartNode->left_child, SearchOrigin, Radius, Targets );
    } else {
        SphereSearchNode( StartNode->right_child, SearchOrigin, Radius, Targets );
    }

    const float distToSiblings = median[StartNode->axis] - SearchOrigin[StartNode->axis];
    if ( Radius * Radius > distToSiblings * distToSiblings ) {
        if ( SearchOrigin[StartNode->axis] < median[StartNode->axis] ) {
            SphereSearchNode( StartNode->right_child, SearchOrigin, Radius, Targets );
        } else {
            SphereSearchNode( StartNode->left_child, SearchOrigin, Radius, Targets );
        }
    }
}
