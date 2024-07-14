
#ifndef MATH_HPP
#define MATH_HPP
#pragma once

#include <xmmintrin.h>

struct vec4 {
    vec4( float Value ) noexcept;
    vec4( float X, float Y, float Z, float W ) noexcept;

    float& operator[]( int i ) noexcept;
    float operator[]( int i ) const noexcept;
    bool operator==( const vec4& rhs ) const noexcept;

#if defined( _MSC_VER )
#pragma warning( push, 3 )
#pragma warning( disable : 4201 )
#endif
    union {
        float a[4] = { 0 };
        struct {
            float x;
            float y;
            float z;
            float w;
        };
    };
#if defined( _MSC_VER )
#pragma warning( default : 4201 )
#pragma warning( pop )
#endif
};

inline vec4 vec_set( vec4& vec0, const vec4& vec1 ) noexcept {
    vec0[0] = vec1[0];
    vec0[1] = vec1[1];
    vec0[2] = vec1[2];
    vec0[3] = vec1[3];

    return vec0;
}

inline vec4 vec_set_f( vec4& vec, const float x, const float y, const float z ) noexcept {
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
    vec[3] = 0.f;

    return vec;
}

inline vec4 vec_zero( vec4& vec ) noexcept {
    vec[0] = 0.f;
    vec[1] = 0.f;
    vec[2] = 0.f;
    vec[3] = 0.f;

    return vec;
}

inline vec4 vec_add( const vec4& vec0, const vec4& vec1 ) noexcept {
    vec4 result( 0.f );

    __m128 v0 = _mm_load_ps( vec0.a );
    __m128 v1 = _mm_load_ps( vec1.a );

    __m128 a0 = _mm_add_ps( v0, v1 );

    _mm_store_ps( result.a, a0 );
    return result;
}

inline vec4 vec_sub( vec4& vec0, vec4& vec1 ) noexcept {
    vec4 result( 0.f );

    __m128 v0 = _mm_load_ps( vec0.a );
    __m128 v1 = _mm_load_ps( vec1.a );

    __m128 a0 = _mm_sub_ps( v0, v1 );

    _mm_store_ps( result.a, a0 );
    return result;
}

inline vec4 vec_mul_f( const vec4& vec, const float f ) noexcept {
    vec4 result( 0.f );

    __m128 v0 = _mm_load_ps( vec.a );
    __m128 v1 = _mm_set_ps( f, f, f, f );

    __m128 a0 = _mm_mul_ps( v0, v1 );

    _mm_store_ps( result.a, a0 );

    return result;
}

inline vec4 vec_divide_f( const vec4& vec, const float f ) noexcept {
    vec4 result( 0.f );

    __m128 v0 = _mm_load_ps( vec.a );
    __m128 v1 = _mm_set_ps( f, f, f, f );

    __m128 a0 = _mm_div_ps( v0, v1 );

    _mm_store_ps( result.a, a0 );

    return result;
}

inline float vec_length_squared( const vec4& vec ) noexcept {
    float result = 0.f;

    __m128 v0 = _mm_load_ps( vec.a );

    __m128 a0 = _mm_mul_ps( v0, v0 );
    __m128 a1 = _mm_add_ps( _mm_shuffle_ps( a0, a0, _MM_SHUFFLE( 2, 3, 0, 1 ) ), a0 );
    __m128 a2 = _mm_add_ps( _mm_shuffle_ps( a1, a1, _MM_SHUFFLE( 1, 0, 3, 2 ) ), a1 );

    _mm_store_ss( &result, a2 );

    return result;
}

inline float vec_length( const vec4& vec ) noexcept {
    float result = 0.f;

    __m128 v0 = _mm_load_ps( vec.a );

    __m128 a0 = _mm_mul_ps( v0, v0 );
    __m128 a1 = _mm_add_ps( _mm_shuffle_ps( a0, a0, _MM_SHUFFLE( 2, 3, 0, 1 ) ), a0 );
    __m128 a2 = _mm_add_ps( _mm_shuffle_ps( a1, a1, _MM_SHUFFLE( 1, 0, 3, 2 ) ), a1 );

    __m128 a3 = _mm_sqrt_ps( a2 );

    _mm_store_ss( &result, a3 );

    return result;
}

inline float vec_distance_squared( const vec4& vec0, const vec4& vec1 ) noexcept {
    float result = 0.f;

    __m128 v0 = _mm_load_ps( vec0.a );
    __m128 v1 = _mm_load_ps( vec1.a );

    __m128 v2 = _mm_sub_ps( v1, v0 );

    __m128 a0 = _mm_mul_ps( v2, v2 );
    __m128 a1 = _mm_add_ps( _mm_shuffle_ps( a0, a0, _MM_SHUFFLE( 2, 3, 0, 1 ) ), a0 );
    __m128 a2 = _mm_add_ps( _mm_shuffle_ps( a1, a1, _MM_SHUFFLE( 1, 0, 3, 2 ) ), a1 );

    _mm_store_ss( &result, a2 );

    return result;
}

inline float vec_distance( const vec4& vec0, const vec4& vec1 ) noexcept {
    float result = 0.f;

    __m128 v0 = _mm_load_ps( vec0.a );
    __m128 v1 = _mm_load_ps( vec1.a );

    __m128 v2 = _mm_sub_ps( v1, v0 );

    __m128 a0 = _mm_mul_ps( v2, v2 );
    __m128 a1 = _mm_add_ps( _mm_shuffle_ps( a0, a0, _MM_SHUFFLE( 2, 3, 0, 1 ) ), a0 );
    __m128 a2 = _mm_add_ps( _mm_shuffle_ps( a1, a1, _MM_SHUFFLE( 1, 0, 3, 2 ) ), a1 );

    __m128 a3 = _mm_sqrt_ps( a2 );

    _mm_store_ss( &result, a3 );

    return result;
}

#endif