
#ifndef TIMER_HPP
#define TIMER_HPP
#pragma once

#include <chrono>
#include <string>
#include "trace.hpp"
#include <fmt/core.h>

struct Timer {
    std::chrono::time_point< std::chrono::steady_clock > start;
    std::chrono::time_point< std::chrono::steady_clock > end;
    std::chrono::duration< double, std::micro > duration;

    void Start() {
        start = std::chrono::steady_clock::now();
    }
    void End( std::string message ) {
        end = std::chrono::steady_clock::now();
        duration = end - start;
        Trace::Instance().Message( fmt::format( "{}: {}", message, duration.count() ), FILENAME, LINENUMBER );
    }
};

#endif
