
// std includes

// System headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

// Local headers
#include "graphics.hpp"
#include "super_waddle/super_waddle.hpp"
#include "trace.hpp"
#include "engine.hpp"
#include "input.hpp"
#include "object_manager.hpp"
#include "component.hpp"
#include "model_manager.hpp"
#include "camera.hpp"
#include "shader_manager.hpp"

static const char* CastToString( const unsigned char* input ) {
    return reinterpret_cast< const char* >( input );
}

Graphics::Graphics() {
}

bool Graphics::Initialize() {
    if ( !glfwInit() ) {
        Trace::Instance().Message( "Could not start GLFW.", FILENAME, LINENUMBER );
        return false;
    }

    // Set core window options (adjust version numbers if needed)
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    // Enable the GLFW runtime error callback function defined previously.
    glfwSetErrorCallback( Graphics::GLFWErrorCallback );

    // Set additional window options
    glfwWindowHint( GLFW_RESIZABLE, windowResizable );
    glfwWindowHint( GLFW_SAMPLES, windowSamples ); // MSAA

    // Create window using GLFW
    window = glfwCreateWindow( windowWidth,
                               windowHeight,
                               windowTitle.c_str(),
                               nullptr,
                               nullptr );

    // Ensure the window is set up correctly
    if ( !window ) {
        Trace::Instance().Message( "Could not open GLFW window.", FILENAME, LINENUMBER );

        glfwTerminate();
        return false;
    }

    // Let the window be the current OpenGL context and initialise glad
    glfwMakeContextCurrent( window );
    gladLoadGL();

    Trace::Instance().Message( fmt::format( "{}: {}",
                                            CastToString( glGetString( GL_VENDOR ) ),
                                            CastToString( glGetString( GL_RENDERER ) ) ),
                               FILENAME, LINENUMBER );
    Trace::Instance().Message( fmt::format( "GLFW\t {}",
                                            glfwGetVersionString() ),
                               FILENAME, LINENUMBER );
    Trace::Instance().Message( fmt::format( "OpenGL\t {}",
                                            CastToString( glGetString( GL_VERSION ) ) ),
                               FILENAME, LINENUMBER );
    Trace::Instance().Message( fmt::format( "GLSL\t {}",
                                            CastToString( glGetString( GL_SHADING_LANGUAGE_VERSION ) ) ),
                               FILENAME, LINENUMBER );

    // Enable depth (Z) buffer (accept "closest" fragment)
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );

    // Configure miscellaneous OpenGL settings
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glFrontFace( GL_CCW );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glPointSize( 3.0 );

    // Set default colour after clearing the colour buffer
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClearStencil( 0 );

    // Set callbacks
    glfwSetFramebufferSizeCallback( window, Graphics::FrameBufferSizeCallback );
    glfwSetCursorEnterCallback( window, Graphics::CursorEnterCallback );
    glfwSetWindowCloseCallback( window, Input::CloseWindowCallback );

    return true;
}

void Graphics::Update() {
    glm::mat4 view = Camera::Instance().GetViewMatrix();
    // TODO: setup shaders
    for ( const auto& [key, value] : ShaderManager::Instance().GetShaderList() ) {
        glUseProgram( value );
        glUniformMatrix4fv( glGetUniformLocation( value, "view" ), 1, GL_FALSE, &view[0][0] );
        glUseProgram( 0 );
    }

    // Clear colour and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    // Draw your scene here
    std::vector< Object >& objectList = ObjectManager::Instance().GetObjectList();
    for ( Object& object : objectList ) {
        Model* model = object.GetComponent< Model >();
        if ( !model ) {
            continue;
        }

        Transform* transform = object.GetComponent< Transform >();

        glm::mat4 modelMatrix = glm::mat4( 1.f );
        modelMatrix = glm::translate( modelMatrix, transform->GetPosition() );
        modelMatrix = glm::rotate( modelMatrix,
                                   glm::radians( transform->GetRotation().x ),
                                   glm::vec3( 1, 0, 0 ) );
        modelMatrix = glm::rotate( modelMatrix,
                                   glm::radians( transform->GetRotation().y ),
                                   glm::vec3( 0, 1, 0 ) );
        modelMatrix = glm::rotate( modelMatrix,
                                   glm::radians( transform->GetRotation().z ),
                                   glm::vec3( 0, 0, 1 ) );
        modelMatrix = glm::scale( modelMatrix, transform->GetScale() );

        glUseProgram( model->GetShader() );

        glUniformMatrix4fv( glGetUniformLocation( model->GetShader(), "model" ),
                            1, GL_FALSE, &modelMatrix[0][0] );

        glm::mat4 projection = glm::perspective< float >( glm::radians( 45.f ),
                                                          windowWidth / windowHeight,
                                                          0.1f, 100.0f );

        glUniformMatrix4fv( glGetUniformLocation( model->GetShader(), "projection" ),
                            1, GL_FALSE, &projection[0][0] );

        glBindVertexArray( model->GetMesh()->VAO );

        glDrawArrays( model->GetRenderMethod(), 0, model->GetMesh()->num_vertices );

        glUseProgram( 0 );

        glBindVertexArray( 0 );
    }

    // Flip buffers
    glfwSwapBuffers( window );

    // Handle other events
    glfwPollEvents();
}

void Graphics::Shutdown() {
    // Terminate GLFW (no need to call glfwDestroyWindow)
    glfwTerminate();
}

GLFWwindow* Graphics::GetWindow() const {
    return window;
}

void Graphics::FrameBufferSizeCallback( GLFWwindow*, int Width, int Height ) {
    glViewport( 0, 0, Width, Height );
}

void Graphics::CursorEnterCallback( GLFWwindow*, int Entered ) {
    if ( Entered ) {
        // The cursor entered the content area of the window
        // glfwSetInputMode( Graphics::Instance().window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
        // cursorEntered = true;
    } else {
        // The cursor left the content area of the window
    }
}

void Graphics::GLFWErrorCallback( int Error, const char* Description ) {
    std::string message = "GLFW returned an error: " +
                          std::string( Description ) +
                          std::to_string( Error );

    Trace::Instance().Message( message, FILENAME, LINENUMBER );
}

Graphics& Graphics::Instance() {
    static Graphics graphicsInstance;
    return graphicsInstance;
}

void handleKeyboardInput( GLFWwindow* window ) {
    // Use escape key for terminating the GLFW window
    if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS ) {
        glfwSetWindowShouldClose( window, GL_TRUE );
        Engine::Instance().TriggerShutdown();
    }
}
