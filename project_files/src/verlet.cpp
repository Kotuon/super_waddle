
// System headers
#include <GLFW/glfw3.h>

// Local includes
#include "verlet.hpp"
#include "object_manager.hpp"
#include "model_manager.hpp"
#include "shader_manager.hpp"
#include "trace.hpp"
#include "input.hpp"
#include "engine.hpp"

void VerletManager::CreateVerlets() {
    Input::Instance().AddCallback( GLFW_KEY_V, &AddVerlet );

    unsigned instanceShader = ShaderManager::Instance().GetShader( "shaders/instance_vertex.glsl",
                                                                   "shaders/instance_fragment.glsl" );

    float distance = 4.f;

    for ( unsigned i = 0; i < max; ++i ) {
        glm::vec3 startPosition{ glm::sin( i ) * distance,
                                 rand() % ( 2 ) + 1,
                                 glm::cos( i ) * distance };

        verlet_list[i] = ObjectManager::Instance().CreateObject(
            std::vector< Component* >{ new Transform( startPosition,
                                                      glm::vec3( 0.15f ),
                                                      glm::vec3( 0.f ) ),
                                       new Physics,
                                       ModelManager::Instance().GetModel( "models/sphere.obj",
                                                                          instanceShader,
                                                                          true ) },
            "Verlet", false );

        verlet_list[i]->GetComponent< Transform >()->SetOldPosition( { glm::sin( i ) * distance * 0.999f,
                                                                       startPosition.y,
                                                                       glm::cos( i ) * distance * 0.999f } );
    }
}

void VerletManager::AddVerlet() {
    VerletManager& instance = VerletManager::Instance();

    instance.timer += Engine::Instance().GetDeltaTime();
    if ( instance.timer < 0.25f ) {
        return;
    }

    if ( instance.curr_count >= instance.max ) {
        return;
    }

    for ( unsigned i = instance.curr_count; i < instance.curr_count + instance.amount_to_add; ++i ) {
        instance.verlet_list[i]->SetIsAlive( true );
    }

    instance.curr_count += instance.amount_to_add;
    instance.timer = 0.f;
}

void VerletManager::UpdateVerlets() {
    if ( curr_count <= 0 ) {
        return;
    }

    for ( unsigned i = 0; i < curr_count - 1; ++i ) {
        for ( unsigned j = i + 1; j < curr_count; ++j ) {
            Transform* t1 = verlet_list[i]->GetComponent< Transform >();
            Transform* t2 = verlet_list[j]->GetComponent< Transform >();

            float dist = glm::distance( t1->GetPosition(), t2->GetPosition() );
            if ( dist < 0.3f ) {
                glm::vec3 norm = glm::normalize( t1->GetPosition() - t2->GetPosition() );
                float delta = 0.3f - dist;
                norm *= 0.5f * delta;
                t1->SetPosition( t1->GetPosition() + norm );
                t2->SetPosition( t2->GetPosition() - norm );
            }
        }
    }

    Transform* ct = container->GetComponent< Transform >();
    float cRadius = 6.f;
    for ( unsigned i = 0; i < curr_count; i++ ) {
        Transform* t1 = verlet_list[i]->GetComponent< Transform >();
        glm::vec3 disp = t1->GetPosition() - ct->GetPosition();
        float dist = glm::length( disp );

        if ( dist > ( cRadius - 0.15f ) ) {
            glm::vec3 norm = glm::normalize( disp );
            norm *= ( cRadius - 0.15f );
            t1->SetPosition( ct->GetPosition() + norm );
        }
    }
}

void VerletManager::PhysicsUpdate() {
    for ( unsigned i = 0; i < curr_count; ++i ) {
        Transform* t = verlet_list[i]->GetComponent< Transform >();

        float dt = Engine::Instance().GetFixedTimeStep();

        glm::vec3 temp = t->GetPosition();
        t->SetPosition( temp * 2.f - t->GetOldPosition() +
                        glm::vec3( 0.f, GRAVITY, 0.f ) * dt * dt );
        t->SetOldPosition( temp );
    }
}

void VerletManager::DrawVerlets( glm::mat4& Projection ) {
    if ( curr_count <= 0 ) {
        return;
    }

    int positionCounter = 0;
    int velocityCounter = 0;

    for ( unsigned i = 0; i < curr_count; ++i ) {
        Transform* transform = verlet_list[i]->GetComponent< Transform >();

        glm::vec3 position = transform->GetPosition();

        positions[positionCounter++] = position.x;
        positions[positionCounter++] = position.y;
        positions[positionCounter++] = position.z;

        glm::vec3 oldPosition = transform->GetOldPosition();
        velocities[velocityCounter++] = glm::distance( position, oldPosition ) * 10.f;
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

void VerletManager::SetContainer( Object* Container ) {
    container = Container;
}

unsigned VerletManager::GetCurrCount() const {
    return curr_count;
}

VerletManager& VerletManager::Instance() {
    static VerletManager verletManagerInstance;
    return verletManagerInstance;
}
