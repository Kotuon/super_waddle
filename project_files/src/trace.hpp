
#ifndef TRACE_HPP
#define TRACE_HPP
#pragma once

#define FILENAME __FILE__
#define LINENUMBER __LINE__

// std includes //
#include <string>
#include <fstream>
#include <source_location>

/*! Trace class */
class Trace {
public:
    /**
     * @brief Prints a message into the output file
     *
     * @param message Message to be printed
     * @param src Source location information
     * @return void
     */
    static void Message( std::string message,
                         std::source_location src = std::source_location::current() );

    /**
     * @brief Destroy the Trace:: Trace object
     *
     */
    ~Trace();

private:
    /**
     * @brief Construct a new Trace:: Trace object
     *
     */
    Trace();

    /**
     * @brief Get the instance of trace
     *
     * @return Trace&
     */
    static Trace& Instance();

private:
    std::fstream trace_stream; //!< Output file
};

#endif
