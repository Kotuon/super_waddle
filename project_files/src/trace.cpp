
// std includes //
#include <iostream>
#include <cstdarg>

// Engine includes //
#include "trace.hpp"

Trace::Trace() {
    trace_stream.open( "trace.log", std::ofstream::out );
    if ( !trace_stream )
        std::cout << "Trace file wasn't opened successfully.\n";
    std::cout << "Trace file was opened successfully.\n";
}

Trace& Trace::Instance() {
    static Trace trace_instance;
    return trace_instance;
}

void Trace::Message( std::string message, std::string filename, int linenumber ) {
    Trace& instance = Trace::Instance();
    if ( !instance.trace_stream )
        return;

    size_t startPos = filename.find_last_of( '\\' );
    if ( startPos == std::string::npos ) {
        startPos = 0;
    }
    std::string output = filename.substr( startPos + 1, std::string::npos ) +
                         "( " + std::to_string( linenumber ) + " )" + ": " + message;

    instance.trace_stream << output << "\n";
    std::cout << output << "\n";
}

Trace::~Trace() {
    if ( trace_stream )
        trace_stream.close();
}
