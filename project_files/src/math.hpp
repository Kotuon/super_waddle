
#ifndef MATH_HPP
#define MATH_HPP
#pragma once

#include <xmmintrin.h>

#define VEC3 4

inline float* vec_set( float* vec0, const float* vec1 ) {
    vec0[0] = vec1[0];
    vec0[1] = vec1[1];
    vec0[2] = vec1[2];

    return vec0;
}

inline float* vec_set_f( float* vec, const float x, const float y, const float z ) {
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;

    return vec;
}

inline float* vec_zero( float* vec ) {
    vec[0] = 0.f;
    vec[1] = 0.f;
    vec[2] = 0.f;

    return vec;
}

inline float* vec_add( float* result, const float* vec0, const float* vec1 ) {
    __m128 v0 = _mm_load_ps( vec0 );
    __m128 v1 = _mm_load_ps( vec1 );

    __m128 a0 = _mm_add_ps( v0, v1 );

    _mm_store_ps( result, a0 );
    return result;
}

inline float* vec_sub( float* result, const float* vec0, const float* vec1 ) {
    __m128 v0 = _mm_load_ps( vec0 );
    __m128 v1 = _mm_load_ps( vec1 );

    __m128 a0 = _mm_sub_ps( v0, v1 );

    _mm_store_ps( result, a0 );
    return result;
}

inline float* vec_mul_f( float* result, const float* vec, const float f ) {
    float vec_f[3] = { f, f, f };

    __m128 v0 = _mm_load_ps( vec );
    __m128 v1 = _mm_load_ps( vec_f );

    __m128 a0 = _mm_mul_ps( v0, v1 );

    _mm_store_ps( result, a0 );

    return result;
}

inline float* vec_divide_f( float* result, const float* vec, const float f ) {
    float vec_f[3] = { f, f, f };

    __m128 v0 = _mm_load_ps( vec );
    __m128 v1 = _mm_load_ps( vec_f );

    __m128 a0 = _mm_div_ps( v0, v1 );

    _mm_store_ps( result, a0 );

    return result;
}

inline float vec_length_squared( const float* vec ) {
    float result = 0.f;

    __m128 v0 = _mm_load_ps( vec );

    __m128 a0 = _mm_mul_ps( v0, v0 );
    __m128 a1 = _mm_add_ps( _mm_shuffle_ps( a0, a0, _MM_SHUFFLE( 2, 3, 0, 1 ) ), a0 );
    __m128 a2 = _mm_add_ps( _mm_shuffle_ps( a1, a1, _MM_SHUFFLE( 1, 0, 3, 2 ) ), a1 );

    _mm_store_ss( &result, a2 );

    return result;
}

inline float vec_length( const float* vec ) {
    float result = 0.f;

    __m128 v0 = _mm_load_ps( vec );

    __m128 a0 = _mm_mul_ps( v0, v0 );
    __m128 a1 = _mm_add_ps( _mm_shuffle_ps( a0, a0, _MM_SHUFFLE( 2, 3, 0, 1 ) ), a0 );
    __m128 a2 = _mm_add_ps( _mm_shuffle_ps( a1, a1, _MM_SHUFFLE( 1, 0, 3, 2 ) ), a1 );

    __m128 a3 = _mm_sqrt_ps( a2 );

    _mm_store_ss( &result, a3 );

    return result;
}

inline float vec_distance( const float* vec0, const float* vec1 ) {
    float result = 0.f;

    __m128 v0 = _mm_load_ps( vec0 );
    __m128 v1 = _mm_load_ps( vec1 );

    __m128 v2 = _mm_sub_ps( v1, v0 );

    __m128 a0 = _mm_mul_ps( v2, v2 );
    __m128 a1 = _mm_add_ps( _mm_shuffle_ps( a0, a0, _MM_SHUFFLE( 2, 3, 0, 1 ) ), a0 );
    __m128 a2 = _mm_add_ps( _mm_shuffle_ps( a1, a1, _MM_SHUFFLE( 1, 0, 3, 2 ) ), a1 );

    __m128 a3 = _mm_sqrt_ps( a2 );

    _mm_store_ss( &result, a3 );

    return result;
}

#endif