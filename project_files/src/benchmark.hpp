
#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP
#pragma once

#include <functional>
#include "timer.hpp"

struct Benchmark {
    Benchmark( int IterationCount ) : iteration_count( IterationCount ) {
    }

    template < typename T >
    void Run( const char* Name, T&& func ) {
        timer.Start();

        for ( int i = 0; i < iteration_count; ++i ) {
            func();
        }

        timer.End( Name );
    }

    Timer timer;
    int iteration_count;
};

#endif
