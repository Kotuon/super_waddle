
#ifndef TRACE_HPP
#define TRACE_HPP
#pragma once

#define FILENAME __FILE__
#define LINENUMBER __LINE__

// std includes //
#include <string>
#include <fstream>

/*! Trace class */
class Trace {
public:
    /**
     * @brief Get the instance of trace
     * 
     * @return Trace& 
     */
    static Trace& Instance();

    /**
     * @brief Prints a message into the output file
     *
     * @param message Message to be printed
     * @param filename Should always be FILENAME
     * @param linenumber Should always be LINENUMBER
     * @return void
     */
    void Message( std::string message, std::string filename, int linenumber );

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

private:
    std::fstream trace_stream; //!< Output file
};

#endif
