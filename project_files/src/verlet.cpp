
// System headers
#include <GLFW/glfw3.h>
#include <fmt/core.h>

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
    dt = Engine::Instance().GetFixedTimeStep();

    for ( unsigned i = 0; i < MAX; ++i ) {
        verlet_list[i] = std::make_unique< Verlet >();

        verlet_list[i]->position = { glm::sin( i ) * distance,
                                     rand() % ( 2 ) + 1,
                                     glm::cos( i ) * distance };
        verlet_list[i]->old_position = { glm::sin( i ) * distance * 0.999f,
                                         verlet_list[i]->position.y,
                                         glm::cos( i ) * distance * 0.999f };
        verlet_list[i]->acceleration = { 0.f, GRAVITY, 0.f };
        verlet_list[i]->radius = 0.15f;
    }
}

void VerletManager::AddVerlet() {
    VerletManager& instance = VerletManager::Instance();

    instance.timer += Engine::Instance().GetDeltaTime();
    if ( instance.timer < 0.1f ) {
        return;
    }

    if ( instance.curr_count >= instance.MAX ) {
        return;
    }

    instance.curr_count += instance.amount_to_add;
    instance.timer = 0.f;
}

void VerletManager::CollisionUpdate() {
    if ( curr_count <= 0 ) {
        return;
    }

    ClearGrid();
    FillGrid();

    for ( unsigned x = 1; x < DIM - 1; ++x ) {
        for ( unsigned y = 1; y < DIM - 1; ++y ) {
            for ( unsigned z = 1; z < DIM - 1; ++z ) {
                // Verlet** currentCell = &collision_grid[x + y * DIM + z * DIM * DIM];
                Verlet** currentCell = collision_grid[x][y][z].data();

                if ( !currentCell[0] ) {
                    continue;
                }

                for ( int dx = -1; dx <= 1; ++dx ) {
                    for ( int dy = -1; dy <= 1; ++dy ) {
                        for ( int dz = -1; dz <= 1; ++dz ) {
                            // Verlet** otherCell = &collision_grid[( x + dx ) + ( y + dy ) * DIM +
                            //                                      ( z + dz ) * DIM * DIM];
                            Verlet** otherCell = collision_grid[x + dx][y + dy][z + dz].data();

                            if ( !otherCell[0] ) {
                                continue;
                            }
                            GridCollision( currentCell, otherCell );
                        }
                    }
                }
            }
        }
    }

    Transform* ct = container->GetComponent< Transform >();
    float cRadius = 6.f;
    for ( unsigned i = 0; i < curr_count; i++ ) {
        Verlet* a = verlet_list[i].get();

        glm::vec3 disp = a->position - ct->GetPosition();
        float dist = glm::length( disp );

        if ( dist > ( cRadius - a->radius ) ) {
            glm::vec3 norm = glm::normalize( disp );
            norm *= ( cRadius - a->radius );
            a->position = ct->GetPosition() + norm;
        }
    }
}

void VerletManager::GridCollision( Verlet** CurrentCell, Verlet** OtherCell ) {
    for ( int a = 0; CurrentCell[a]; ++a ) {
        for ( int b = 0; OtherCell[b]; ++b ) {
            Verlet* v1 = CurrentCell[a];
            Verlet* v2 = OtherCell[b];

            if ( v1 != v2 ) {
                VerletCollision( v1, v2 );
            }
        }
    }
}

void VerletManager::VerletCollision( Verlet* a, Verlet* b ) {
    float dist = glm::distance( a->position, b->position );
    if ( dist < a->radius + b->radius ) {
        glm::vec3 norm = glm::normalize( a->position - b->position );
        float delta = a->radius + b->radius - dist;
        norm *= 0.5f * delta;
        a->position += norm;
        b->position -= norm;
    }
}

void VerletManager::PositionUpdate() {
    for ( unsigned i = 0; i < curr_count; ++i ) {
        Verlet* a = verlet_list[i].get();

        glm::vec3 temp = a->position;
        a->position += a->position - a->old_position + a->acceleration * dt * dt;
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

void VerletManager::ClearGrid() {
    // collision_grid.fill( nullptr );
    memset( collision_grid.data(), 0, DIM * DIM * DIM * CELL_MAX * ( sizeof( Verlet* ) ) );
}

void VerletManager::FillGrid() {
    for ( unsigned i = 0; i < curr_count; ++i ) {
        Verlet* obj = verlet_list[i].get();
        int x = static_cast< int >( obj->position.x / ( obj->radius * 2 ) + DIM / 2 );
        int y = static_cast< int >( obj->position.y / ( obj->radius * 2 ) + DIM / 2 );
        int z = static_cast< int >( obj->position.z / ( obj->radius * 2 ) + DIM / 2 );

        x = glm::clamp< int >( x, 0, DIM - 1 );
        y = glm::clamp< int >( y, 0, DIM - 1 );
        z = glm::clamp< int >( z, 0, DIM - 1 );

        InsertNode( x, y, z, obj );
    }
}

void VerletManager::InsertNode( int x, int y, int z, Verlet* obj ) {
    // Verlet** currentCell = &collision_grid[x + y * DIM + z * DIM * DIM];
    Verlet** currentCell = collision_grid[x][y][z].data();

    int i = 0;
    while ( currentCell[i] ) {
        i += 1;
    }
    if ( i >= CELL_MAX ) {
        Trace::Instance().Message( fmt::format( "x: {}, y: {}, z:{}", obj->position.x, obj->position.y, obj->position.z ), FILENAME, LINENUMBER );
        Trace::Instance().Message( fmt::format( "x: {}, y: {}, z:{}", x, y, z ), FILENAME, LINENUMBER );
        Trace::Instance().Message( fmt::format( "Bad index: {}.", i ), FILENAME, LINENUMBER );
    }
    // collision_grid[x + y * DIM + z * DIM * DIM + i * DIM * DIM * DIM] = obj;
    collision_grid[x][y][z][i] = obj;
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
