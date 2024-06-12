
// Standard headers
#include <cstdlib>
#include <windows.h>

// System headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>

// Local headers
#include "super_waddle/super_waddle.hpp"
#include "graphics.hpp"
#include "engine.hpp"
#include "trace.hpp"
#include "crash_handler.hpp"

#include "math.hpp"
#include <glm/glm.hpp>
#include <functional>
#include "benchmark.hpp"

int main( int, char*[] ) {
    SetupDump();

    // char dir[256];
    // GetModuleFileName( nullptr, dir, 256 );
    // Trace::Instance().Message( fmt::format( "{}", dir ), FILENAME, LINENUMBER );

    // // Initialize application
    // bool result = Engine::Instance().Initialize();
    // if ( !result ) {
    //     return EXIT_FAILURE;
    // }

    // // Running application
    // Engine::Instance().Update();

    // // Shutdown application
    // Engine::Instance().Shutdown();

    float x0 = static_cast< float >( rand() ) / ( static_cast< float >( RAND_MAX / 9.f ) );
    float y0 = static_cast< float >( rand() ) / ( static_cast< float >( RAND_MAX / 9.f ) );
    float z0 = static_cast< float >( rand() ) / ( static_cast< float >( RAND_MAX / 9.f ) );

    float x1 = static_cast< float >( rand() ) / ( static_cast< float >( RAND_MAX / 9.f ) );
    float y1 = static_cast< float >( rand() ) / ( static_cast< float >( RAND_MAX / 9.f ) );
    float z1 = static_cast< float >( rand() ) / ( static_cast< float >( RAND_MAX / 9.f ) );

    glm::vec3 g0( x0, y0, z0 );
    glm::vec3 g1( x1, y1, z1 );
    glm::vec3 g2;

    float v0[VEC3];
    vec_set_f( v0, x0, y0, z0 );

    float v1[VEC3];
    vec_set_f( v1, x1, y1, z1 );

    float v2[VEC3];

    Benchmark b( 100000 );

    b.Run( "GLM: Length", [g0]() {
        glm::length( g0 );
    } );

    b.Run( "SIMD: Length", [v0]() {
        vec_length( v0 );
    } );

    b.Run( "GLM: Distance", [g0, g1]() {
        glm::distance( g0, g1 );
    } );

    b.Run( "SIMD: Distance", [v0, v1]() {
        vec_distance( v0, v1 );
    } );

    b.Run( "GLM: Add", [g0, g1, g2]() mutable {
        g2 = g0 + g1;
    } );

    b.Run( "SIMD: Add", [v0, v1, v2]() mutable {
        vec_add( v2, v0, v1 );
    } );

    b.Run( "GLM: Sub", [g0, g1, g2]() mutable {
        g2 = g0 - g1;
    } );

    b.Run( "SIMD: Sub", [v0, v1, v2]() mutable {
        vec_sub( v2, v0, v1 );
    } );

    b.Run( "GLM: Mul", [g0, g1, g2]() mutable {
        g2 = g0 * 5.3f;
    } );

    b.Run( "SIMD: Mul", [v0, v1, v2]() mutable {
        vec_mul_f( v2, v0, 5.3f );
    } );

    b.Run( "GLM: Divide", [g0, g1, g2]() mutable {
        g2 = g0 / 5.3f;
    } );

    b.Run( "SIMD: Divide", [v0, v1, v2]() mutable {
        vec_divide_f( v2, v0, 5.3f );
    } );

    b.Run( "GLM: Set F", [g0, x0, y0, z0]() mutable {
        g0 = { x0, y0, z0 };
    } );

    b.Run( "SIMD: Set F", [v0, x0, y0, z0]() mutable {
        vec_set_f( v0, x0, y0, z0 );
    } );

    b.Run( "GLM: Set Vec", [g0, g1]() mutable {
        g0 = g1;
    } );

    b.Run( "SIMD: Set Vec", [v0, v1]() mutable {
        vec_set( v0, v1 );
    } );

    return EXIT_SUCCESS;
}
