
// std includes
#include <fstream>

// System includes
#include <glad/glad.h>
#include <fmt/core.h>

// Local includes
#include "shader_manager.hpp"
#include "trace.hpp"

unsigned ShaderManager::GetShader( const std::string& VertexFile, const std::string& FragmentFile ) {
    auto it = shader_list.find( VertexFile + FragmentFile );
    if ( it != shader_list.end() ) {
        return it->second;
    }

    GLint success = 0;
    GLint logSize = 0;

    unsigned vertexShader = glCreateShader( GL_VERTEX_SHADER );
    const char* vertexShaderSource = ReadFile( VertexFile );
    if ( !vertexShaderSource ) {
        return 0;
    }

    glShaderSource( vertexShader, 1, &vertexShaderSource, nullptr );
    glCompileShader( vertexShader );

    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &success );
    if ( success == GL_FALSE ) {
        glGetShaderiv( vertexShader, GL_INFO_LOG_LENGTH, &logSize );
        GLchar* infoLog = new GLchar[logSize];
        glGetShaderInfoLog( vertexShader, logSize, &logSize, infoLog );
        glDeleteShader( vertexShader );
        Trace::Message( fmt::format( "Vertex Shader {}: {}\n", VertexFile, infoLog ) );
        delete[] infoLog;
    }

    unsigned fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
    const char* fragmentShaderSource = ReadFile( FragmentFile );
    if ( !fragmentShaderSource ) {
        return 0;
    }

    glShaderSource( fragmentShader, 1, &fragmentShaderSource, nullptr );
    glCompileShader( fragmentShader );

    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &success );
    if ( success == GL_FALSE ) {
        glGetShaderiv( fragmentShader, GL_INFO_LOG_LENGTH, &logSize );
        GLchar* infoLog = new GLchar[logSize];
        glGetShaderInfoLog( fragmentShader, logSize, &logSize, infoLog );
        glDeleteShader( fragmentShader );
        Trace::Message( fmt::format( "Fragment Shader {}: {}\n", FragmentFile, infoLog ) );
        delete[] infoLog;
    }

    unsigned shaderProgram = glCreateProgram();
    glAttachShader( shaderProgram, vertexShader );
    glAttachShader( shaderProgram, fragmentShader );
    glLinkProgram( shaderProgram );

    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    shader_list[VertexFile + FragmentFile] = shaderProgram;
    program_list[shaderProgram] = VertexFile + FragmentFile;

    return shaderProgram;
}

void ShaderManager::DetachShader() {
    glUseProgram( 0 );
}

void ShaderManager::DestroyShader( unsigned ShaderID ) {
    glDeleteProgram( ShaderID );

    auto iter = program_list.find( ShaderID );
    shader_list.erase( iter->second );

    program_list.erase( ShaderID );
}

const char* ShaderManager::ReadFile( const std::string& FileName ) {
    auto it = source_list.find( FileName );
    if ( it != source_list.end() ) {
        return it->second.c_str();
    }

    std::string& content = source_list[FileName];

    std::ifstream file( FileName );
    if ( !file.is_open() ) {
        Trace::Message( fmt::format( "Failed to open shader {}.", FileName ) );

        source_list.erase( FileName );
        return nullptr;
    }

    std::string line = "";
    while ( !file.eof() ) {
        getline( file, line );
        content.append( line + "\n" );
    }

    file.close();

    content.append( "\0" );

    return content.c_str();
}

const std::unordered_map< std::string, unsigned >& ShaderManager::GetShaderList() const {
    return shader_list;
}

ShaderManager& ShaderManager::Instance() {
    static ShaderManager shaderManagerInstance;
    return shaderManagerInstance;
}
