
// std includes
#include <functional>
#include <algorithm>

// System headers
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <glm/ext/matrix_transform.hpp>
// #include "imgui.h"

// Local includes
#include "verlet.hpp"
#include "model_manager.hpp"
#include "shader_manager.hpp"
#include "trace.hpp"
#include "input.hpp"
#include "engine.hpp"
#include "graphics.hpp"
#include "editor.hpp"
#include "octree.hpp"
#include "timer.hpp"

static inline unsigned findex( const unsigned index ) noexcept {
    return index * fvec::VEC4_SIZE;
}

void VerletManager::CreateVerlets( const ContainerShape CShape ) {
    Trace::Message( fmt::format( "Thread count: {}", THREAD_COUNT ) );
    for ( int i = 0; i < THREAD_COUNT; ++i ) {
        threads.emplace_back();
    }

    projection = Graphics::Instance().GetProjection();

    Graphics::Instance().AddRenderCallback( std::bind( &VerletManager::DrawVerlets, this ) );
    Engine::Instance().AddFixedUpdateCallback( std::bind( &VerletManager::Update, this ) );
    // Editor::Instance().AddDisplayMenuCallback( std::bind( &VerletManager::DisplayMenu, this ) );

    Input::Instance().AddCallback( GLFW_KEY_V, std::bind( &VerletManager::AddVerlet, this ) );
    Input::Instance().AddCallback( GLFW_KEY_G, std::bind( &VerletManager::ApplyForce, this ) );
    Input::Instance().AddCallback( GLFW_KEY_H, std::bind( &VerletManager::ToggleForce, this ) );

    unsigned instance_shader = ShaderManager::Instance().GetShader( "shaders/instance_vertex.glsl",
                                                                    "shaders/instance_fragment.glsl" );

    model = ModelManager::Instance().GetModel( "models/sphere.obj", instance_shader, true );

    dt = Engine::Instance().GetFixedTimeStep();

    octree = std::make_unique< Octree >();
    octree->SetVerletCollisionCallback( std::bind(
        &VerletManager::CheckCollisionBetweenVerlets, this,
        std::placeholders::_1, std::placeholders::_2 ) );

    SetupContainer( CShape );
    SetupVerlets();
}

void VerletManager::SetupVerletPosition( const unsigned i ) {
    const float x = static_cast< float >( glm::sin( findex( i ) ) *
                                          ( container.collision_radius * ( 2.f / 3.f ) ) );
    const float y = static_cast< float >( rand() % ( 2 ) + 1 );
    const float z = static_cast< float >( glm::cos( findex( i ) ) *
                                          ( container.collision_radius * ( 2.f / 3.f ) ) );

    fvec::set_f( &f_pos[findex( i )], x, y, z );
    fvec::set_f( &f_oldpos[findex( i )], x * 0.999f, y, z * 0.999f );
    fvec::set_zero( &f_accel[findex( i )] );
}

void VerletManager::SetupVerlets() {
    curr_count = 0;

    for ( unsigned i = 0; i < MAX; ++i ) {
        SetupVerletPosition( i );
    }
}

void VerletManager::SetupContainer( const ContainerShape CShape ) {
    const unsigned cShader = ShaderManager::Instance().GetShader( "shaders/base_vertex.glsl",
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

    if ( ( 1.f / Engine::Instance().GetDeltaTime() ) < fps_limit ) {
        return;
    }

    curr_count = std::clamp( curr_count + amount_to_add, static_cast< unsigned >( 0 ), MAX );
    add_timer = 0.f;
}

void VerletManager::RemoveVerlet() {
    if ( add_timer < add_cooldown || curr_count <= 0 ) {
        return;
    }

    const unsigned lastCount = curr_count;
    curr_count = std::clamp( curr_count - amount_to_add, static_cast< unsigned >( 0 ), MAX );
    add_timer = 0.f;

    for ( unsigned i = lastCount; i > curr_count; --i ) {
        SetupVerletPosition( i );
    }
}

void VerletManager::ApplyForce() {
    if ( force_toggle ) {
        return;
    }

    for ( unsigned i = 0; i < curr_count; ++i ) {
        float disp[fvec::VEC4_SIZE];
        fvec::sub( disp, &f_pos[findex( i )], force_position );

        const float dist = fvec::len( disp );
        if ( dist > 0.f ) {
            float norm[fvec::VEC4_SIZE];
            fvec::div_f( norm, disp, dist );
            fvec::mul_f( norm, norm, -30.f );
            fvec::add( &f_accel[findex( i )], &f_accel[findex( i )], norm );
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

static Timer timer;
void VerletManager::Update() {
    add_timer += Engine::Instance().GetDeltaTime();
    toggle_timer += Engine::Instance().GetDeltaTime();

    if ( !should_simulate ) {
        return;
    }

    if ( curr_count <= 0 ) {
        return;
    }

    // octree->ClearTree();
    // octree->FillTree( f_pos, verlet_radius, curr_count );

    timer.Run( "Clear tree",
               [this]() { octree->ClearTree(); } );

    timer.Run( "Fill tree",
               [this]() { octree->FillTree( f_pos, verlet_radius, curr_count ); } );

    timer.Run( "Verlet collisions",
               [this]() { octree->CheckCollisions(); } );

    timer.Run( "Container collision",
               [this]() { ContainerCollision(); } );

    timer.Run( "Position update",
               [this]() { PositionUpdate(); } );

    // ContainerCollision();
    // PositionUpdate();

    // for ( int i = 0; i < THREAD_COUNT; ++i ) {
    //     threads[i] = std::thread( &VerletManager::PositionUpdateThread, this, i );
    // }
    // for ( std::thread& thd : threads ) {
    //     thd.join();
    // }
}

void VerletManager::CheckCollisionBetweenVerlets( const unsigned* Verlet1, const unsigned* Verlet2 ) noexcept {
    if ( Verlet1 != Verlet2 ) {
        float axis[fvec::VEC4_SIZE];
        fvec::sub( axis, &f_pos[findex( *Verlet1 )], &f_pos[findex( *Verlet2 )] );

        float dist = fvec::len( axis );
        if ( dist < verlet_diameter ) {
            float norm[fvec::VEC4_SIZE];
            fvec::div_f( norm, axis, dist );

            float delta = verlet_diameter - dist;
            fvec::mul_f( norm, norm, 0.5f * delta );

            fvec::add( &f_pos[findex( *Verlet1 )], &f_pos[findex( *Verlet1 )], norm );
            fvec::sub( &f_pos[findex( *Verlet2 )], &f_pos[findex( *Verlet2 )], norm );
        }
    }
}

void VerletManager::ContainerCollision() {
    switch ( container.shape ) {
    case Sphere:
        for ( unsigned i = 0; i < curr_count; ++i ) {
            float disp[fvec::VEC4_SIZE];
            fvec::set( disp, &f_pos[findex( i )] );

            const float dist = fvec::len( disp );
            if ( dist > ( container.collision_radius - verlet_radius ) ) {
                float norm[fvec::VEC4_SIZE];
                fvec::div_f( norm, disp, dist );

                fvec::mul_f( &f_pos[findex( i )], norm, container.collision_radius - verlet_radius );
            }
        }
        break;

    case Cube:
        for ( unsigned i = 0; i < curr_count; ++i ) {
            for ( unsigned j = 0; j < 3; ++j ) {
                if ( f_pos[findex( i ) + j] < -container.collision_radius ) {
                    const float dist = f_pos[findex( i ) + j] - f_oldpos[findex( i ) + j];

                    f_pos[findex( i ) + j] = -container.collision_radius;
                    f_oldpos[findex( i ) + j] = f_pos[findex( i ) + j] + dist;
                } else if ( f_pos[findex( i ) + j] > container.collision_radius ) {
                    const float dist = f_pos[findex( i ) + j] - f_oldpos[findex( i ) + j];

                    f_pos[findex( i ) + j] = container.collision_radius;
                    f_oldpos[findex( i ) + j] = f_pos[findex( i ) + j] + dist;
                }
            }
        }
        break;

    default:
        break;
    }
}

void VerletManager::PositionUpdateThread( const int ThreadId ) noexcept {
    const unsigned start = ThreadId * ( curr_count / THREAD_COUNT );
    const unsigned end = ( ThreadId == THREAD_COUNT - 1 )
                             ? curr_count
                             : ( ThreadId + 1 ) * ( curr_count / THREAD_COUNT );

    for ( unsigned i = start; i < end; ++i ) {
        if ( force_toggle ) {
            float disp[fvec::VEC4_SIZE];
            fvec::sub( disp, &f_pos[findex( i )], force_position );

            const float dist = fvec::len( disp );
            if ( dist > 0.f ) {
                float norm[fvec::VEC4_SIZE];
                fvec::div_f( norm, disp, dist );
                fvec::mul_f( norm, norm, -30.f );
                fvec::add( &f_accel[findex( i )], &f_accel[findex( i )], norm );
            }
        }

        fvec::add( &f_accel[findex( i )], &f_accel[findex( i )], grav_vec );

        float temp[fvec::VEC4_SIZE];
        fvec::set( temp, &f_pos[findex( i )] );

        float disp[fvec::VEC4_SIZE];
        fvec::sub( disp, &f_pos[findex( i )], &f_oldpos[findex( i )] );

        fvec::set( &f_oldpos[findex( i )], &f_pos[findex( i )] );

        float forceReduction[fvec::VEC4_SIZE];
        fvec::mul_f( forceReduction, disp, vel_damping );
        fvec::sub( &f_accel[findex( i )], &f_accel[findex( i )], forceReduction );

        fvec::mul_f( &f_accel[findex( i )], &f_accel[findex( i )], dt * dt );
        fvec::add( &f_pos[findex( i )], &f_pos[findex( i )], disp );
        fvec::add( &f_pos[findex( i )], &f_pos[findex( i )], &f_accel[findex( i )] );

        fvec::set_zero( &f_accel[findex( i )] );
    }
}

void VerletManager::PositionUpdate() noexcept {
    for ( unsigned i = 0; i < curr_count; ++i ) {
        if ( force_toggle ) {
            float disp[fvec::VEC4_SIZE];
            fvec::sub( disp, &f_pos[findex( i )], force_position );

            const float dist = fvec::len( disp );
            if ( dist > 0.f ) {
                float norm[fvec::VEC4_SIZE];
                fvec::div_f( norm, disp, dist );
                fvec::mul_f( norm, norm, -30.f );
                fvec::add( &f_accel[findex( i )], &f_accel[findex( i )], norm );
            }
        }

        fvec::add( &f_accel[findex( i )], &f_accel[findex( i )], grav_vec );

        float temp[fvec::VEC4_SIZE];
        fvec::set( temp, &f_pos[findex( i )] );

        float disp[fvec::VEC4_SIZE];
        fvec::sub( disp, &f_pos[findex( i )], &f_oldpos[findex( i )] );

        fvec::set( &f_oldpos[findex( i )], &f_pos[findex( i )] );

        float forceReduction[fvec::VEC4_SIZE];
        fvec::mul_f( forceReduction, disp, vel_damping );
        fvec::sub( &f_accel[findex( i )], &f_accel[findex( i )], forceReduction );

        fvec::mul_f( &f_accel[findex( i )], &f_accel[findex( i )], dt * dt );
        fvec::add( &f_pos[findex( i )], &f_pos[findex( i )], disp );
        fvec::add( &f_pos[findex( i )], &f_pos[findex( i )], &f_accel[findex( i )] );

        fvec::set_zero( &f_accel[findex( i )] );
    }
}

void VerletManager::DrawVerlets() {
    if ( curr_count <= 0 ) {
        Graphics::Instance().DrawNormal( container.model, container.matrix );
        return;
    }

    int velocityCounter = 0;

    for ( unsigned i = 0; i < curr_count; ++i ) {
        velocities[velocityCounter++] = fvec::dist( &f_pos[findex( i )], &f_oldpos[findex( i )] ) *
                                        10.f;
    }

    glBindBuffer( GL_ARRAY_BUFFER, model->GetMesh()->position_VBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float ) * fvec::VEC4_SIZE * curr_count,
                     f_pos.data() );
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

unsigned VerletManager::GetCurrCount() const {
    return curr_count;
}

void VerletManager::DisplayMenu() {
    // ImGui::Begin( "VerletIntegration##1" );

    // ImGui::Text( fmt::format( "Particle count: {}", curr_count ).c_str() );
    // ImGui::SliderFloat( "FPS Limit", &fps_limit, 30.f, 90.f );
    // ImGui::SliderFloat( "Verlet radius", &verlet_radius, 0.15f, 0.5f );

    // ImGui::SeparatorText( "Amount to add" );
    // ImGui::SliderInt( "##1", &amount_to_add, 1, 1000 );

    // ImGui::SeparatorText( "Add cooldown" );
    // ImGui::SliderFloat( "##2", &add_cooldown, 0.1f, 2.f );

    // if ( ImGui::Button( "Add Particles##1" ) ) {
    //     AddVerlet();
    // }

    // if ( ImGui::Button( "Remove Particles##1" ) ) {
    //     RemoveVerlet();
    // }

    // ImGui::Separator();

    // ImGui::Checkbox( "Should simulate##1", &should_simulate );

    // ImGui::SeparatorText( "Forces" );
    // ImGui::SliderFloat3( "Force position", force_position.a, -10.f, 10.f );
    // ImGui::Checkbox( "Toggle force##1", &force_toggle );

    // ImGui::Separator();

    // ImGui::SliderFloat3( "Gravity position", grav_vec.a, -5.f, 5.f );
    // ImGui::SliderFloat( "Velocity damping", &vel_damping, 0.f, 1000.f );

    // ImGui::SeparatorText( "Container shape" );

    // static int currShape = container.shape;
    // static const char* shapeList[2] = { "Sphere", "Cube" };
    // if ( ImGui::Combo( "##3", &currShape, shapeList, 2 ) ) {
    //     SetupContainer( static_cast< ContainerShape >( currShape ) );
    // }

    // if ( ImGui::SliderFloat( "Container size", &container.collision_radius, 2.f, 10.f ) ) {
    //     switch ( container.shape ) {
    //     case Sphere:
    //         container.model_radius = container.collision_radius * 1.02f;
    //         break;
    //     case Cube:
    //         container.model_radius = container.collision_radius * 2.f + 0.15f * 3.f;
    //         break;
    //     }

    //     container.matrix = glm::scale( glm::mat4( 1.f ), { container.model_radius,
    //                                                        container.model_radius,
    //                                                        container.model_radius } );
    // }

    // if ( ImGui::Button( "Reset" ) ) {
    //     SetupVerlets();
    // }

    // ImGui::End();
}

VerletManager& VerletManager::Instance() {
    static VerletManager verletManagerInstance;
    return verletManagerInstance;
}
