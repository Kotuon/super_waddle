
#ifndef FLOAT_MATH_HPP
#define FLOAT_MATH_HPP
#pragma once

#include <xmmintrin.h>

namespace fvec {
constexpr unsigned VEC4_SIZE = 4;

inline void set( float* vec0, const float* vec1 ) noexcept {
    vec0[0] = vec1[0];
    vec0[1] = vec1[1];
    vec0[2] = vec1[2];
    vec0[3] = vec1[3];
}

inline void set_f( float* vec, const float x, const float y, const float z ) noexcept {
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
    vec[3] = 0.f;
}

inline void set_zero( float* vec ) noexcept {
    vec[0] = 0.f;
    vec[1] = 0.f;
    vec[2] = 0.f;
    vec[3] = 0.f;
}

inline void add( float* result, const float* vec0, const float* vec1 ) noexcept {
    __m128 v0 = _mm_load_ps( vec0 );
    __m128 v1 = _mm_load_ps( vec1 );

    __m128 a0 = _mm_add_ps( v0, v1 );

    _mm_store_ps( result, a0 );
}

inline void sub( float* result, const float* vec0, const float* vec1 ) noexcept {
    __m128 v0 = _mm_load_ps( vec0 );
    __m128 v1 = _mm_load_ps( vec1 );

    __m128 a0 = _mm_sub_ps( v0, v1 );

    _mm_store_ps( result, a0 );
}

inline void mul_f( float* result, const float* vec, const float f ) noexcept {
    __m128 v0 = _mm_load_ps( vec );
    __m128 v1 = _mm_set_ps( f, f, f, f );

    __m128 a0 = _mm_mul_ps( v0, v1 );

    _mm_store_ps( result, a0 );
}

inline void div_f( float* result, const float* vec, const float f ) noexcept {
    __m128 v0 = _mm_load_ps( vec );
    __m128 v1 = _mm_set_ps( f, f, f, f );

    __m128 a0 = _mm_div_ps( v0, v1 );

    _mm_store_ps( result, a0 );
}

inline float len_sq( const float* vec ) noexcept {
    float result = 0.f;

    __m128 v0 = _mm_load_ps( vec );

    __m128 a0 = _mm_mul_ps( v0, v0 );
    __m128 a1 = _mm_add_ps( _mm_shuffle_ps( a0, a0, _MM_SHUFFLE( 2, 3, 0, 1 ) ), a0 );
    __m128 a2 = _mm_add_ps( _mm_shuffle_ps( a1, a1, _MM_SHUFFLE( 1, 0, 3, 2 ) ), a1 );

    _mm_store_ss( &result, a2 );

    return result;
}

inline float len( const float* vec ) noexcept {
    float result = 0.f;

    __m128 v0 = _mm_load_ps( vec );

    __m128 a0 = _mm_mul_ps( v0, v0 );
    __m128 a1 = _mm_add_ps( _mm_shuffle_ps( a0, a0, _MM_SHUFFLE( 2, 3, 0, 1 ) ), a0 );
    __m128 a2 = _mm_add_ps( _mm_shuffle_ps( a1, a1, _MM_SHUFFLE( 1, 0, 3, 2 ) ), a1 );

    __m128 a3 = _mm_sqrt_ss( a2 );

    _mm_store_ss( &result, a3 );

    return result;
}

inline float dist_sq( const float* vec0, const float* vec1 ) noexcept {
    float result = 0.f;

    __m128 v0 = _mm_load_ps( vec0 );
    __m128 v1 = _mm_load_ps( vec1 );

    __m128 v2 = _mm_sub_ps( v1, v0 );

    __m128 a0 = _mm_mul_ps( v2, v2 );
    __m128 a1 = _mm_add_ps( _mm_shuffle_ps( a0, a0, _MM_SHUFFLE( 2, 3, 0, 1 ) ), a0 );
    __m128 a2 = _mm_add_ps( _mm_shuffle_ps( a1, a1, _MM_SHUFFLE( 1, 0, 3, 2 ) ), a1 );

    _mm_store_ss( &result, a2 );
    return result;
}

inline float dist( const float* vec0, const float* vec1 ) noexcept {
    float result = 0.f;

    __m128 v0 = _mm_load_ps( vec0 );
    __m128 v1 = _mm_load_ps( vec1 );

    __m128 v2 = _mm_sub_ps( v1, v0 );

    __m128 a0 = _mm_mul_ps( v2, v2 );
    __m128 a1 = _mm_add_ps( _mm_shuffle_ps( a0, a0, _MM_SHUFFLE( 2, 3, 0, 1 ) ), a0 );
    __m128 a2 = _mm_add_ps( _mm_shuffle_ps( a1, a1, _MM_SHUFFLE( 1, 0, 3, 2 ) ), a1 );

    __m128 a3 = _mm_sqrt_ss( a2 );

    _mm_store_ss( &result, a3 );
    return result;
}

}; // namespace fvec

#endif
