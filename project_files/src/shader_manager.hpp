
#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP
#pragma once

// std includes
#include <unordered_map>
#include <string>

class ShaderManager {
public:
    unsigned GetShader( const std::string& VertexFile, const std::string& FragmentFile );
    void DetachShader();
    void DestroyShader( unsigned ShaderID );

    const char* ReadFile( const std::string& FileName );

    const std::unordered_map< std::string, unsigned >& GetShaderList() const;

    static ShaderManager& Instance();

private:
    std::unordered_map< std::string, unsigned > shader_list;
    std::unordered_map< unsigned, std::string > program_list;
    std::unordered_map< std::string, std::string > source_list;
};

#endif
