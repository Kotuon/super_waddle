
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

    instance_shader = ShaderManager::Instance().GetShader( "shaders/instance_vertex.glsl",
                                                           "shaders/instance_fragment.glsl" );

    model = ModelManager::Instance().GetModel( "models/sphere.obj", instance_shader, true );

    float distance = 4.f;

    for ( unsigned i = 0; i < max; ++i ) {
        verlet_list[i] = std::make_unique< Verlet >();

        verlet_list[i]->position = { glm::sin( i ) * distance,
                                     rand() % ( 2 ) + 1,
                                     glm::cos( i ) * distance };
        verlet_list[i]->old_position = { glm::sin( i ) * distance * 0.999f,
                                         verlet_list[i]->position.y,
                                         glm::cos( i ) * distance * 0.999f };
        verlet_list[i]->acceleration = glm::vec3( 0.f );
        verlet_list[i]->radius = 0.15f;
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

    instance.curr_count += instance.amount_to_add;
    instance.timer = 0.f;
}

void VerletManager::UpdateVerlets() {
    if ( curr_count <= 0 ) {
        return;
    }

    for ( unsigned i = 0; i < curr_count - 1; ++i ) {
        for ( unsigned j = i + 1; j < curr_count; ++j ) {
            Verlet* a = verlet_list[i].get();
            Verlet* b = verlet_list[j].get();

            float dist = glm::distance( a->position, b->position );
            if ( dist < 0.3f ) {
                glm::vec3 norm = glm::normalize( a->position - b->position );
                float delta = 0.3f - dist;
                norm *= 0.5f * delta;
                a->position += norm;
                b->position -= norm;
            }
        }
    }

    Transform* ct = container->GetComponent< Transform >();
    float cRadius = 6.f;
    for ( unsigned i = 0; i < curr_count; i++ ) {
        Verlet* a = verlet_list[i].get();

        glm::vec3 disp = a->position - ct->GetPosition();
        float dist = glm::length( disp );

        if ( dist > ( cRadius - 0.15f ) ) {
            glm::vec3 norm = glm::normalize( disp );
            norm *= ( cRadius - 0.15f );
            a->position = ct->GetPosition() + norm;
        }
    }
}

void VerletManager::PhysicsUpdate() {
    for ( unsigned i = 0; i < curr_count; ++i ) {
        Verlet* a = verlet_list[i].get();

        float dt = Engine::Instance().GetFixedTimeStep();
        a->acceleration.y = GRAVITY;

        glm::vec3 temp = a->position;
        a->position += temp - a->old_position + a->acceleration * dt * dt;
        a->old_position = temp;
    }
}

void VerletManager::DrawVerlets( glm::mat4& Projection ) {
    if ( curr_count <= 0 ) {
        return;
    }

    int positionCounter = 0;
    int velocityCounter = 0;

    for ( unsigned i = 0; i < curr_count; ++i ) {
        Verlet* a = verlet_list[i].get();

        positions[positionCounter++] = a->position.x;
        positions[positionCounter++] = a->position.y;
        positions[positionCounter++] = a->position.z;

        velocities[velocityCounter++] = glm::distance( a->position, a->old_position ) * 10.f;
    }

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

    glUniform1f( glGetUniformLocation( model->GetShader(), "scale" ), verlet_list[0]->radius );

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
