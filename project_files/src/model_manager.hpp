
#ifndef MODEL_MANAGER_HPP
#define MODEL_MANAGER_HPP
#pragma once

// std includes
#include <unordered_map>
#include <string>

// System includes
#include <glad/glad.h>

constexpr unsigned VERTEX_LIMIT = 2000;
constexpr unsigned STRIDE = 8;
constexpr unsigned INSTANCE_STRIDE = 3;
constexpr unsigned MAX_INSTANCES = 20000;

struct Mesh {
    std::string model_file_name;
    int num_vertices;
    unsigned VAO;
    unsigned VBO;
    unsigned position_VBO;
    unsigned velocity_VBO;
};

struct Model {
    glm::vec3 position;
    glm::vec3 rotation;
    Mesh* mesh;
    float scale;
    unsigned render_method;
};

class Model_Manager {
public:
    Model* GetModel( Mesh* mesh );
    Mesh* GetMesh( std::string& ModelFileName, bool Instanced );

    static Model_Manager& Instance();

private:
    Model_Manager();
    std::vector< float >* LoadObj( std::string& ModelFileName );

    void InsertData( std::vector< float >& vertices, char* data[3],
                     std::array< glm::vec3, VERTEX_LIMIT > v,
                     std::array< glm::vec3, VERTEX_LIMIT > vt,
                     std::array< glm::vec3, VERTEX_LIMIT > vn );

    std::unordered_map< std::string, std::vector< float > > vertices_list;
    std::unordered_map< std::string, Mesh > mesh_list;
    std::unordered_map< std::string, Model > model_list;
};

#endif
