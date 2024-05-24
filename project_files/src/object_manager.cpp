
// Local includes
#include "object_manager.hpp"
#include "model_manager.hpp"

Object::Object() : id( -1 ), name( "object" ) {
    AddComponent( new Transform );
}

Object::Object( std::string Name ) : id( -1 ), name( Name ) {
    AddComponent( new Transform );
}

Object::Object( const Object& Other ) : id( -1 ), name( Other.name ) {
    Transform* transform = Other.GetComponentConst< Transform >();
    if ( transform ) {
        AddComponent( new Transform( *transform ) );
    }

    // TODO: copy components
}

void Object::Clear() {
    Transform* transform = GetComponent< Transform >();

    if ( transform ) {
        components.erase( CType::CTransform );

        delete transform;
        transform = nullptr;
    }
}

void Object::AddComponent( Component* Component ) {
    Component->SetParent( this );
    components.emplace( Component->GetCType(), Component );
}

void Object::SetId( int Id ) {
    id = Id;
}

int Object::GetId() const {
    return id;
}

void Object::Update() {
    // TODO: Update components
}

bool Object::HasComponent( CType Type ) {
    auto componentIter = components.find( Type );
    if ( componentIter == components.end() ) {
        return false;
    }

    return true;
}

ObjectManager::ObjectManager() {
}

Object& ObjectManager::CreateObject( std::string Name ) {
    object_list.emplace_back( Name );
    Object& newObject = object_list.back();
    newObject.SetId( static_cast< int >( object_list.size() - 1 ) );

    return newObject;
}

Object& ObjectManager::CreateObject( std::vector< Component* > Components, std::string Name ) {
    Object& newObject = CreateObject( Name );

    for ( Component* component : Components ) {
        if ( !newObject.HasComponent( component->GetCType() ) ) {
            newObject.AddComponent( component );
        } else {
            delete component;
            component = nullptr;
        }
    }

    return newObject;
}

void ObjectManager::CreateObjects( std::vector< Component* > Components, std::string Name, int Amount ) {
    for ( int i = 0; i < Amount; ++i ) {
        CreateObject( Components, Name );
    }
}

std::vector< Object >& ObjectManager::GetObjectList() {
    return object_list;
}

ObjectManager& ObjectManager::Instance() {
    static ObjectManager objectManagerInstance;
    return objectManagerInstance;
}
