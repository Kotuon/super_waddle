
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
    }
}

VerletManager& VerletManager::Instance() {
    static VerletManager verletManagerInstance;
    return verletManagerInstance;
}
