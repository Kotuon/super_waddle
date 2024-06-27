
// std includes
#include <functional>
#include <algorithm>

// System headers
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <glm/ext/matrix_transform.hpp>
#include "imgui.h"

// Local includes
#include "verlet.hpp"
#include "model_manager.hpp"
#include "shader_manager.hpp"
#include "trace.hpp"
#include "input.hpp"
#include "engine.hpp"
#include "math.hpp"
#include "graphics.hpp"
#include "editor.hpp"

void VerletManager::CreateVerlets( ContainerShape CShape ) {
    projection = Graphics::Instance().GetProjection();

    Graphics::Instance().AddRenderCallback( std::bind( &VerletManager::DrawVerlets, this ) );
    Engine::Instance().AddFixedUpdateCallback( std::bind( &VerletManager::Update, this ) );
    Editor::Instance().AddDisplayMenuCallback( std::bind( &VerletManager::DisplayMenu, this ) );

    Input::Instance().AddCallback( GLFW_KEY_V, std::bind( &VerletManager::AddVerlet, this ) );
    Input::Instance().AddCallback( GLFW_KEY_G, std::bind( &VerletManager::ApplyForce, this ) );
    Input::Instance().AddCallback( GLFW_KEY_H, std::bind( &VerletManager::ToggleForce, this ) );

    unsigned instance_shader = ShaderManager::Instance().GetShader( "shaders/instance_vertex.glsl",
                                                                    "shaders/instance_fragment.glsl" );

    model = ModelManager::Instance().GetModel( "models/sphere.obj", instance_shader, true );

    dt = Engine::Instance().GetFixedTimeStep();

    SetupContainer( CShape );
    SetupVerlets();
}

void VerletManager::SetupVerletPosition( Verlet* verlet, int i ) {
    float x = static_cast< float >( glm::sin( i ) *
                                    ( container.collision_radius * ( 2.f / 3.f ) ) );
    float y = static_cast< float >( rand() % ( 2 ) + 1 );
    float z = static_cast< float >( glm::cos( i ) *
                                    ( container.collision_radius * ( 2.f / 3.f ) ) );

    vec_set_f( verlet->position, x, y, z );
    vec_set_f( verlet->old_position, x * 0.999f, y, z * 0.999f );
    vec_set_f( verlet->acceleration, 0.f, 0.f, 0.f );
}

void VerletManager::SetupVerlets() {
    curr_count = 0;

    for ( unsigned i = 0; i < MAX; ++i ) {
        if ( !verlet_list[i] ) {
            verlet_list[i] = std::make_unique< Verlet >();
        }

        SetupVerletPosition( verlet_list[i].get(), i );
    }
}

void VerletManager::SetupContainer( ContainerShape CShape ) {
    unsigned cShader = ShaderManager::Instance().GetShader( "shaders/base_vertex.glsl",
                                                            "shaders/base_fragment.glsl" );

    container.shape = CShape;
    switch ( container.shape ) {
    case Sphere:
        container.model = ModelManager::Instance().GetModel( "models/sphere.obj", GL_POINTS,
                                                             cShader, false );
        container.model_radius = container.collision_radius * 1.02f;
        break;
    case Cube:
        container.model = ModelManager::Instance().GetModel( "models/cube.obj", GL_TRIANGLES,
                                                             cShader, false );
        container.model_radius = container.collision_radius * 2.f + 0.15f * 3.f;
        break;
    }

    container.matrix = glm::scale( glm::mat4( 1.f ), { container.model_radius,
                                                       container.model_radius,
                                                       container.model_radius } );
}

void VerletManager::AddVerlet() {
    if ( add_timer < add_cooldown || curr_count >= MAX ) {
        return;
    }

    if ( ( 1.f / Engine::Instance().GetDeltaTime() ) < 90.f ) {
        return;
    }

    curr_count = std::clamp( curr_count + amount_to_add, static_cast< unsigned >( 0 ), MAX );
    add_timer = 0.f;
}

void VerletManager::RemoveVerlet() {
    if ( add_timer < add_cooldown || curr_count <= 0 ) {
        return;
    }

    unsigned lastCount = curr_count;
    curr_count = std::clamp( curr_count - amount_to_add, static_cast< unsigned >( 0 ), MAX );
    add_timer = 0.f;

    for ( unsigned i = lastCount; i > curr_count; --i ) {
        SetupVerletPosition( verlet_list[i].get(), i );
    }
}

void VerletManager::ApplyForce() {
    if ( force_toggle ) {
        return;
    }

    for ( unsigned i = 0; i < curr_count; ++i ) {
        Verlet* v = verlet_list[i].get();

        float disp[VEC3];
        vec_sub( disp, v->position, force_vec );
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
    if ( toggle_timer < 0.5f ) {
        return;
    }

    force_toggle = !force_toggle;
    toggle_timer = 0.f;
}

void VerletManager::Update() {
    add_timer += Engine::Instance().GetDeltaTime();
    toggle_timer += Engine::Instance().GetDeltaTime();

    if ( !should_simulate ) {
        return;
    }

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
                if ( dist < verlet_radius + verlet_radius ) {
                    float norm[VEC3];
                    vec_divide_f( norm, axis, dist );

                    float delta = verlet_radius + verlet_radius - dist;
                    vec_mul_f( norm, norm, 0.5f * delta );
                    vec_add( v1->position, v1->position, norm );
                    vec_sub( v2->position, v2->position, norm );
                }
            }
        }
    }
}

void VerletManager::ContainerCollision() {

    switch ( container.shape ) {
    case Sphere:
        for ( unsigned i = 0; i < curr_count; ++i ) {
            Verlet* v = verlet_list[i].get();

            float disp[VEC3]{ v->position[0], v->position[1], v->position[2] };
            float dist = vec_length( disp );

            if ( dist > ( container.collision_radius - verlet_radius ) ) {
                float norm[VEC3];
                vec_divide_f( norm, disp, dist );
                vec_mul_f( norm, norm, container.collision_radius - verlet_radius );
                vec_set( v->position, norm );
            }
        }
        break;

    case Cube:
        for ( unsigned i = 0; i < curr_count; ++i ) {
            Verlet* v = verlet_list[i].get();

            for ( unsigned j = 0; j < 3; ++j ) {
                if ( v->position[j] < -container.collision_radius ) {
                    float disp = v->position[j] - v->old_position[j];
                    v->position[j] = -container.collision_radius;
                    v->old_position[j] = v->position[j] + disp;
                }
                if ( v->position[j] > container.collision_radius ) {
                    float disp = v->position[j] - v->old_position[j];
                    v->position[j] = container.collision_radius;
                    v->old_position[j] = v->position[j] + disp;
                }
            }
        }
        break;

    default:
        break;
    }
}

void VerletManager::PositionUpdate() {
    for ( unsigned i = 0; i < curr_count; ++i ) {
        Verlet* verlet = verlet_list[i].get();

        if ( force_toggle ) {
            float disp[VEC3];
            vec_sub( disp, verlet->position, force_vec );
            float dist = vec_length( disp );

            if ( dist > 0 ) {
                float norm[VEC3];
                vec_divide_f( norm, disp, dist );
                vec_mul_f( norm, norm, -30.f );
                vec_add( verlet->acceleration, verlet->acceleration, norm );
            }
        }

        vec_add( verlet->acceleration, verlet->acceleration, grav_vec );

        float temp[VEC3]{ verlet->position[0], verlet->position[1], verlet->position[2] };
        float disp[VEC3];
        vec_sub( disp, verlet->position, verlet->old_position );
        vec_set( verlet->old_position, verlet->position );

        float forceReduction[VEC3];
        vec_mul_f( forceReduction, disp, vel_damping );
        vec_sub( verlet->acceleration, verlet->acceleration, forceReduction );

        vec_mul_f( verlet->acceleration, verlet->acceleration, dt * dt );
        vec_add( verlet->position, verlet->position, disp );
        vec_add( verlet->position, verlet->position, verlet->acceleration );

        vec_zero( verlet->acceleration );
    }
}

void VerletManager::DrawVerlets() {
    if ( curr_count <= 0 ) {
        Graphics::Instance().DrawNormal( container.model, container.matrix );
        return;
    }

    int positionCounter = 0;
    int velocityCounter = 0;

    for ( unsigned i = 0; i < curr_count; ++i ) {
        Verlet* verlet = verlet_list[i].get();

        positions[positionCounter++] = verlet->position[0];
        positions[positionCounter++] = verlet->position[1];
        positions[positionCounter++] = verlet->position[2];

        velocities[velocityCounter++] = vec_distance( verlet->position,
                                                      verlet->old_position ) *
                                        10.f;
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
                        1, GL_FALSE, &projection[0][0] );

    glUniform1f( glGetUniformLocation( model->GetShader(), "scale" ), verlet_radius );

    glBindVertexArray( model->GetMesh()->VAO );

    glDrawArraysInstanced( model->GetRenderMethod(), 0, model->GetMesh()->num_vertices,
                           curr_count );

    glUseProgram( 0 );
    glBindVertexArray( 0 );

    Graphics::Instance().DrawNormal( container.model, container.matrix );
}

void VerletManager::ClearGrid() {
    collision_grid.fill( nullptr );
}

void VerletManager::FillGrid() {
    for ( unsigned i = 0; i < curr_count; ++i ) {
        Verlet* verlet = verlet_list[i].get();
        int x = static_cast< int >( verlet->position[0] / ( verlet_radius * 2 ) + DIM / 2 );
        int y = static_cast< int >( verlet->position[1] / ( verlet_radius * 2 ) + DIM / 2 );
        int z = static_cast< int >( verlet->position[2] / ( verlet_radius * 2 ) + DIM / 2 );

        x = glm::clamp< int >( x, 0, DIM - 1 );
        y = glm::clamp< int >( y, 0, DIM - 1 );
        z = glm::clamp< int >( z, 0, DIM - 1 );

        InsertNode( x, y, z, verlet );
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

void VerletManager::DisplayMenu() {
    ImGui::Begin( "VerletIntegration##1" );

    ImGui::Text( fmt::format( "Particle count: {}", curr_count ).c_str() );
    ImGui::SliderFloat( "Verlet radius", &verlet_radius, 0.15f, 0.5f );

    ImGui::SeparatorText( "Amount to add" );
    ImGui::SliderInt( "##1", &amount_to_add, 1, 1000 );

    ImGui::SeparatorText( "Add cooldown" );
    ImGui::SliderFloat( "##2", &add_cooldown, 0.1f, 2.f );

    if ( ImGui::Button( "Add Particles##1" ) ) {
        AddVerlet();
    }

    if ( ImGui::Button( "Remove Particles##1" ) ) {
        RemoveVerlet();
    }

    ImGui::Separator();

    ImGui::Checkbox( "Should simulate##1", &should_simulate );

    ImGui::SeparatorText( "Forces" );
    ImGui::SliderFloat3( "Force position", force_vec, -10.f, 10.f );
    ImGui::Checkbox( "Toggle force##1", &force_toggle );

    ImGui::Separator();

    ImGui::SliderFloat3( "Gravity position", grav_vec, -5.f, 5.f );
    ImGui::SliderFloat( "Velocity damping", &vel_damping, 0.f, 100.f );

    ImGui::SeparatorText( "Container shape" );

    static int currShape = container.shape;
    static const char* shapeList[2] = { "Sphere", "Cube" };
    if ( ImGui::Combo( "##3", &currShape, shapeList, 2 ) ) {
        SetupContainer( static_cast< ContainerShape >( currShape ) );
    }

    if ( ImGui::SliderFloat( "Container size", &container.collision_radius, 2.f, 10.f ) ) {
        switch ( container.shape ) {
        case Sphere:
            container.model_radius = container.collision_radius * 1.02f;
            break;
        case Cube:
            container.model_radius = container.collision_radius * 2.f + 0.15f * 3.f;
            break;
        }

        container.matrix = glm::scale( glm::mat4( 1.f ), { container.model_radius,
                                                           container.model_radius,
                                                           container.model_radius } );
    }

    if ( ImGui::Button( "Reset" ) ) {
        SetupVerlets();
    }

    ImGui::End();
}

VerletManager& VerletManager::Instance() {
    static VerletManager verletManagerInstance;
    return verletManagerInstance;
}
