
// std includes
#include <cassert>

// Local includes
#include "math.hpp"

vec4::vec4( float Value ) noexcept : x( Value ), y( Value ), z( Value ), w( Value ) {
}

vec4::vec4( float X, float Y, float Z, float W ) noexcept : x( X ),
                                                            y( Y ),
                                                            z( Z ),
                                                            w( W ) {
}

float& vec4::operator[]( int i ) noexcept {
    assert( i < 4 );
    return a[i];
}

float vec4::operator[]( int i ) const noexcept {
    assert( i < 4 );
    return a[i];
}

bool vec4::operator==( const vec4& rhs ) const noexcept {
    for ( int i = 0; i < 4; ++i ) {
        if ( a[i] != rhs.a[i] ) {
            return false;
        }
    }

    return true;
}
