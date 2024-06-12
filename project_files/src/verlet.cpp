
#include <chrono>

// System headers
#include <GLFW/glfw3.h>
#include <fmt/core.h>

#include <Eigen/Dense>

// Local includes
#include "verlet.hpp"
#include "model_manager.hpp"
#include "shader_manager.hpp"
#include "trace.hpp"
#include "input.hpp"
#include "engine.hpp"
#include "math.hpp"

struct Timer {
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;
    std::chrono::duration< double, std::micro > duration;

    void Start() {
        start = std::chrono::high_resolution_clock::now();
    }
    void End( std::string message ) {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        Trace::Instance().Message( fmt::format( "{}: {}", message, duration.count() ), FILENAME, LINENUMBER );
    }
};

static Timer timer;

void VerletManager::CreateVerlets() {
    Input::Instance().AddCallback( GLFW_KEY_V, &AddVerlet );
    Input::Instance().AddCallback( GLFW_KEY_G, &ApplyForce );
    Input::Instance().AddCallback( GLFW_KEY_H, &ToggleForce );

    unsigned instance_shader = ShaderManager::Instance().GetShader( "shaders/instance_vertex.glsl",
                                                                    "shaders/instance_fragment.glsl" );

    model = ModelManager::Instance().GetModel( "models/sphere.obj", instance_shader, true );

    float distance = 4.f;
    dt = Engine::Instance().GetFixedTimeStep();

    for ( unsigned i = 0; i < MAX; ++i ) {
        verlet_list[i] = std::make_unique< Verlet >();

        float x = static_cast< float >( glm::sin( i ) * distance );
        float y = static_cast< float >( rand() % ( 2 ) + 1 );
        float z = static_cast< float >( glm::cos( i ) * distance );

        vec_set_f( verlet_list[i]->position, x, y, z );
        vec_set_f( verlet_list[i]->old_position, x * 0.999f, y, z * 0.999f );
        vec_set_f( verlet_list[i]->acceleration, 0.f, 0.f, 0.f );

        scales[i] = verlet_list[i]->radius;
    }
}

void VerletManager::AddVerlet() {
    VerletManager& instance = VerletManager::Instance();

    if ( instance.add_timer < 0.1f || instance.curr_count >= instance.MAX ) {
        return;
    }

    if ( ( 1.f / Engine::Instance().GetDeltaTime() ) < 90.f ) {
        return;
    }

    instance.curr_count += instance.amount_to_add;
    instance.add_timer = 0.f;
}

void VerletManager::ApplyForce() {
    VerletManager& instance = VerletManager::Instance();
    if ( instance.force_toggle ) {
        return;
    }

    for ( unsigned i = 0; i < instance.curr_count; ++i ) {
        Verlet* v = instance.verlet_list[i].get();

        float disp[VEC3];
        vec_sub( disp, v->position, instance.force_vec );
        float dist = vec_length( disp );

        if ( dist > 0 ) {
            float norm[VEC3];
            vec_divide_f( norm, disp, dist );
            vec_mul_f( norm, norm, -30.f );
            vec_add( v->acceleration, v->acceleration, norm );
        }
    }
}

void VerletManager::ToggleForce() {
    VerletManager& instance = VerletManager::Instance();
    if ( instance.toggle_timer < 0.5f ) {
        return;
    }

    instance.force_toggle = !instance.force_toggle;
    instance.toggle_timer = 0.f;
}

void VerletManager::Update() {
    add_timer += Engine::Instance().GetDeltaTime();
    toggle_timer += Engine::Instance().GetDeltaTime();

    CollisionUpdate();
    PositionUpdate();
}

void VerletManager::CollisionUpdate() {
    if ( curr_count <= 0 ) {
        return;
    }

    ClearGrid();
    FillGrid();

    for ( int i = 0; i < THREAD_COUNT; ++i ) {
        threads[i] = std::thread( &VerletManager::GridCollisionThread, this, i );
    }
    for ( std::thread& thd : threads ) {
        thd.join();
    }

    ContainerCollision();
}

void VerletManager::GridCollision() {
    for ( unsigned x = 1; x < DIM - 1; ++x ) {
        for ( unsigned y = 1; y < DIM - 1; ++y ) {
            for ( unsigned z = 1; z < DIM - 1; ++z ) {
                Verlet** currentCell = &collision_grid[( z + y * DIM + x * DIM * DIM ) * CELL_MAX];

                if ( !currentCell[0] ) {
                    continue;
                }

                for ( int dx = -1; dx <= 1; ++dx ) {
                    for ( int dy = -1; dy <= 1; ++dy ) {
                        for ( int dz = -1; dz <= 1; ++dz ) {
                            Verlet** otherCell = &collision_grid[( ( z + dz ) + ( y + dy ) * DIM +
                                                                   ( x + dx ) * DIM * DIM ) *
                                                                 CELL_MAX];

                            if ( !otherCell[0] ) {
                                continue;
                            }
                            VerletCollision( currentCell, otherCell );
                        }
                    }
                }
            }
        }
    }
}

void VerletManager::GridCollisionThread( int ThreadId ) {
    unsigned start = 1 + ThreadId * ( ( DIM ) / THREAD_COUNT );
    unsigned end = 1 + ( ThreadId + 1 ) * ( ( DIM ) / THREAD_COUNT );

    if ( ThreadId == THREAD_COUNT - 1 ) {
        end += DIM % THREAD_COUNT - 2;
    }

    for ( unsigned x = start; x < end; ++x ) {
        for ( unsigned y = 1; y < DIM - 1; ++y ) {
            for ( unsigned z = 1; z < DIM - 1; ++z ) {
                Verlet** currentCell = &collision_grid[( z + y * DIM + x * DIM * DIM ) * CELL_MAX];

                if ( !currentCell[0] ) {
                    continue;
                }

                for ( int dx = -1; dx <= 1; ++dx ) {
                    for ( int dy = -1; dy <= 1; ++dy ) {
                        for ( int dz = -1; dz <= 1; ++dz ) {
                            Verlet** otherCell = &collision_grid[( ( z + dz ) + ( y + dy ) * DIM +
                                                                   ( x + dx ) * DIM * DIM ) *
                                                                 CELL_MAX];

                            if ( !otherCell[0] ) {
                                continue;
                            }
                            VerletCollision( currentCell, otherCell );
                        }
                    }
                }
            }
        }
    }
}

void VerletManager::VerletCollision( Verlet** CurrentCell, Verlet** OtherCell ) {
    for ( int a = 0; CurrentCell[a]; ++a ) {
        for ( int b = 0; OtherCell[b]; ++b ) {
            Verlet* v1 = CurrentCell[a];
            Verlet* v2 = OtherCell[b];

            if ( v1 != v2 ) {
                float axis[VEC3];
                vec_sub( axis, v1->position, v2->position );
                float dist = vec_length( axis );
                if ( dist < v1->radius + v2->radius ) {
                    float norm[VEC3];
                    vec_divide_f( norm, axis, dist );

                    float delta = v1->radius + v2->radius - dist;
                    vec_mul_f( norm, norm, 0.5f * delta );
                    vec_add( v1->position, v1->position, norm );
                    vec_sub( v2->position, v2->position, norm );
                }
            }
        }
    }
}

void VerletManager::ContainerCollision() {
    for ( unsigned i = 0; i < curr_count; i++ ) {
        Verlet* a = verlet_list[i].get();

        float disp[VEC3]{ a->position[0], a->position[1], a->position[2] };
        float dist = vec_length( disp );

        if ( dist > ( c_radius - a->radius ) ) {
            float norm[VEC3];
            vec_divide_f( norm, disp, dist );
            vec_mul_f( norm, norm, c_radius - a->radius );
            vec_set( a->position, norm );
        }
    }
}

void VerletManager::PositionUpdate() {
    for ( unsigned i = 0; i < curr_count; ++i ) {
        Verlet* a = verlet_list[i].get();

        if ( force_toggle ) {
            float disp[VEC3];
            vec_sub( disp, a->position, force_vec );
            float dist = vec_length( disp );

            if ( dist > 0 ) {
                float norm[VEC3];
                vec_divide_f( norm, disp, dist );
                vec_mul_f( norm, norm, -30.f );
                vec_add( a->acceleration, a->acceleration, norm );
            }
        }

        vec_add( a->acceleration, a->acceleration, grav_vec );

        float temp[VEC3]{ a->position[0], a->position[1], a->position[2] };
        float disp[VEC3];
        vec_sub( disp, a->position, a->old_position );
        vec_set( a->old_position, a->position );

        vec_mul_f( a->acceleration, a->acceleration, dt * dt );
        vec_add( a->position, a->position, disp );
        vec_add( a->position, a->position, a->acceleration );

        vec_zero( a->acceleration );
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

        positions[positionCounter++] = a->position[0];
        positions[positionCounter++] = a->position[1];
        positions[positionCounter++] = a->position[2];

        velocities[velocityCounter++] = vec_distance( a->position, a->old_position ) * 10.f;
    }

    glBindBuffer( GL_ARRAY_BUFFER, model->GetMesh()->position_VBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * 3 * curr_count,
                     positions.data() );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, model->GetMesh()->velocity_VBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * curr_count, velocities.data() );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, model->GetMesh()->scale_VBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * curr_count, scales.data() );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glUseProgram( model->GetShader() );

    glUniformMatrix4fv( glGetUniformLocation( model->GetShader(), "projection" ),
                        1, GL_FALSE, &Projection[0][0] );

    // glUniform1f( glGetUniformLocation( model->GetShader(), "scale" ), verlet_list[0]->radius );

    glBindVertexArray( model->GetMesh()->VAO );

    glDrawArraysInstanced( model->GetRenderMethod(), 0, model->GetMesh()->num_vertices,
                           curr_count );

    glUseProgram( 0 );
    glBindVertexArray( 0 );
}

void VerletManager::ClearGrid() {
    collision_grid.fill( nullptr );
}

void VerletManager::FillGrid() {
    for ( unsigned i = 0; i < curr_count; ++i ) {
        Verlet* obj = verlet_list[i].get();
        int x = static_cast< int >( obj->position[0] / ( obj->radius * 2 ) + DIM / 2 );
        int y = static_cast< int >( obj->position[1] / ( obj->radius * 2 ) + DIM / 2 );
        int z = static_cast< int >( obj->position[2] / ( obj->radius * 2 ) + DIM / 2 );

        x = glm::clamp< int >( x, 0, DIM - 1 );
        y = glm::clamp< int >( y, 0, DIM - 1 );
        z = glm::clamp< int >( z, 0, DIM - 1 );

        InsertNode( x, y, z, obj );
    }
}

void VerletManager::InsertNode( int x, int y, int z, Verlet* obj ) {
    Verlet** currentCell = &collision_grid[( z + y * DIM + x * DIM * DIM ) * CELL_MAX];

    int i = 0;
    while ( currentCell[i] ) {
        i += 1;
    }
    collision_grid[i + ( z + y * DIM + x * DIM * DIM ) * CELL_MAX] = obj;
}

unsigned VerletManager::GetCurrCount() const {
    return curr_count;
}

void VerletManager::SetContainerRadius( float Radius ) {
    c_radius = Radius;
}

VerletManager& VerletManager::Instance() {
    static VerletManager verletManagerInstance;
    return verletManagerInstance;
}
