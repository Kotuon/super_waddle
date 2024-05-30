
// Local includes
#include "verlet.hpp"
#include "object_manager.hpp"
#include "model_manager.hpp"
#include "shader_manager.hpp"
#include "trace.hpp"

void VerletManager::CreateVerlets( int Amount ) {
    unsigned instanceShader = ShaderManager::Instance().GetShader( "shaders/instance_vertex.glsl",
                                                                   "shaders/instance_fragment.glsl" );

    for ( int i = 0; i < Amount; ++i ) {
        Object* object = ObjectManager::Instance().CreateObject(
            std::vector< Component* >{ new Transform,
                                       new Physics,
                                       ModelManager::Instance().GetModel( "models/sphere.obj", instanceShader,
                                                                          false ) },
            "Verlet" );

        verlet_list.push_back( object );
        for ( int j = 0; j < 3; ++j ) {
            positions.push_back( 0.f );
        }
        velocities.push_back( 0.f );
    }
}

void VerletManager::UpdateVerlets() {
}

void VerletManager::DrawVerlets( glm::mat4& Projection ) {
    int positionCounter = 0;
    int velocityCounter = 0;

    for ( Object* verlet : verlet_list ) {
        Transform* transform = verlet->GetComponent< Transform >();

        glm::vec3 position = transform->GetPosition();

        positions[positionCounter++] = position.x;
        positions[positionCounter++] = position.y;
        positions[positionCounter++] = position.z;

        glm::vec3 oldPosition = transform->GetOldPosition();
        velocities[velocityCounter++] = glm::distance( position, oldPosition );
    }

    Transform* transform = verlet_list[0]->GetComponent< Transform >();
    Model* model = verlet_list[0]->GetComponent< Model >();

    glBindBuffer( GL_ARRAY_BUFFER, model->GetMesh()->position_VBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * 3 * verlet_list.size(),
                     positions.data() );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, model->GetMesh()->velocity_VBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * verlet_list.size(), velocities.data() );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glUseProgram( model->GetShader() );

    glUniformMatrix4fv( glGetUniformLocation( model->GetShader(), "projection" ),
                        1, GL_FALSE, &Projection[0][0] );

    glUniform1f( glGetUniformLocation( model->GetShader(), "scale" ), transform->GetScale().x );

    glBindVertexArray( model->GetMesh()->VAO );

    glDrawArraysInstanced( model->GetRenderMethod(), 0, model->GetMesh()->num_vertices,
                           static_cast< int >( verlet_list.size() ) );

    glUseProgram( 0 );
    glBindVertexArray( 0 );
}

VerletManager& VerletManager::Instance() {
    static VerletManager verletManagerInstance;
    return verletManagerInstance;
}
