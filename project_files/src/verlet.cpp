
// System headers
#include <GLFW/glfw3.h>

// Local includes
#include "verlet.hpp"
#include "object_manager.hpp"
#include "model_manager.hpp"
#include "shader_manager.hpp"
#include "trace.hpp"
#include "input.hpp"

void VerletManager::CreateVerlets() {
    Input::Instance().AddCallback( GLFW_KEY_V, &AddVerlet );

    unsigned instanceShader = ShaderManager::Instance().GetShader( "shaders/instance_vertex.glsl",
                                                                   "shaders/instance_fragment.glsl" );

    for ( int i = 0; i < max; ++i ) {
        glm::vec3 startPosition{ glm::sin( i ) * 7.f,
                                 rand() % ( 2 ) + 1,
                                 glm::cos( i ) * 7.f };

        Object* object = ObjectManager::Instance().CreateObject(
            std::vector< Component* >{ new Transform( startPosition,
                                                      glm::vec3( 0.15f ),
                                                      glm::vec3( 0.f ) ),
                                       new Physics,
                                       ModelManager::Instance().GetModel( "models/sphere.obj",
                                                                          instanceShader,
                                                                          true ) },
            "Verlet" );

        object->GetComponent< Transform >()->SetOldPosition( { glm::sin( i ) * 7.f * 0.999f,
                                                               startPosition.y,
                                                               glm::cos( i ) * 7.f * 0.999f } );

        verlet_list.push_back( object );
        for ( int j = 0; j < 3; ++j ) {
            positions.push_back( 0.f );
        }
        velocities.push_back( 0.f );
    }
}

void VerletManager::AddVerlet() {
    VerletManager& instance = VerletManager::Instance();

    if ( instance.curr_count >= instance.max ) {
        return;
    }

    instance.curr_count += 1;
}

void VerletManager::UpdateVerlets() {
}

void VerletManager::DrawVerlets( glm::mat4& Projection ) {
    if ( curr_count <= 0 ) {
        return;
    }

    int positionCounter = 0;
    int velocityCounter = 0;

    for ( int i = 0; i < curr_count; ++i ) {
        Transform* transform = verlet_list[i]->GetComponent< Transform >();

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
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * 3 * curr_count,
                     positions.data() );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, model->GetMesh()->velocity_VBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * curr_count, velocities.data() );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glUseProgram( model->GetShader() );

    glUniformMatrix4fv( glGetUniformLocation( model->GetShader(), "projection" ),
                        1, GL_FALSE, &Projection[0][0] );

    glUniform1f( glGetUniformLocation( model->GetShader(), "scale" ), transform->GetScale().x );

    glBindVertexArray( model->GetMesh()->VAO );

    glDrawArraysInstanced( model->GetRenderMethod(), 0, model->GetMesh()->num_vertices,
                           curr_count );

    glUseProgram( 0 );
    glBindVertexArray( 0 );
}

VerletManager& VerletManager::Instance() {
    static VerletManager verletManagerInstance;
    return verletManagerInstance;
}
