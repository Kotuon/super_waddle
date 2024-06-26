
#ifndef MODEL_MANAGER_HPP
#define MODEL_MANAGER_HPP
#pragma once

// std includes
#include <unordered_map>
#include <string>

// System includes
#include <glad/glad.h>
#include <glm/glm.hpp>

constexpr unsigned VERTEX_LIMIT = 2000;
constexpr unsigned STRIDE = 8;
constexpr unsigned INSTANCE_STRIDE = 3;
constexpr unsigned MAX_INSTANCES = 40000;

struct Mesh {
    Mesh();
    Mesh( std::string ModelFileName );

    std::string model_file_name;
    int num_vertices;
    unsigned VAO;
    unsigned VBO;
    unsigned position_VBO;
    unsigned velocity_VBO;
    bool instanced;
};

class Model {
public:
    Model( Mesh* NewMesh, unsigned Shader );
    Model( Mesh* NewMesh, unsigned RenderMethod, unsigned Shader );
    void Draw();

    void SetMesh( Mesh* NewMesh );
    Mesh* GetMesh() const;

    void SetRenderMethod( unsigned RenderMethod );
    unsigned GetRenderMethod() const;

    void SetShader( unsigned NewShader );
    unsigned GetShader() const;

private:
    Model();
    Mesh* mesh;
    unsigned render_method;
    unsigned shader;
};

class ModelManager {
public:
    Model* GetModel( const std::string& ModelFileName, unsigned Shader, bool Instanced );
    Model* GetModel( const std::string& ModelFileName, unsigned RenderMethod,
                     unsigned Shader, bool Instanced );

    static ModelManager& Instance();

private:
    ModelManager();

    Mesh* GetMesh( const std::string& ModelFileName, bool Instanced );

    std::vector< float >* LoadObj( const std::string& ModelFileName );

    void InsertData( std::vector< float >& vertices, char* data[3],
                     std::array< glm::vec3, VERTEX_LIMIT >& v,
                     std::array< glm::vec3, VERTEX_LIMIT >& vt,
                     std::array< glm::vec3, VERTEX_LIMIT >& vn );

    std::unordered_map< std::string, std::vector< float > > vertices_list;
};

#endif
