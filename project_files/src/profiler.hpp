
#ifndef PROFILER_HPP
#define PROFILER_HPP
#pragma once

#include <vector>
#include <chrono>
#include <Windows.h>
#include <dbghelp.h>
#include <thread>

class Profiler {
public:
    Profiler();
    ~Profiler();

    void Record();
    PSYMBOL_INFO GetSymbol( DWORD64, PSYMBOL_INFO );

    int find( std::vector< std::tuple< DWORD64, char*, int > >&, char*, int );
    void StartFile();
    void CloseFile();
    void WriteMessage( std::tuple< DWORD64, char*, int >& );

private:
    std::vector< CONTEXT > eip_list;
    HANDLE main_thread;
    std::chrono::time_point< std::chrono::steady_clock > start;
    std::chrono::time_point< std::chrono::steady_clock > current;
    FILE* debug_file = NULL;
    std::thread t1;
    bool hasWritten = false;
    bool exit = false;
};

#endif
