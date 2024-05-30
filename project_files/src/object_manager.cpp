
// System includes
#include <fmt/core.h>

// Local includes
#include "object_manager.hpp"
#include "model_manager.hpp"
#include "trace.hpp"

Object::Object() : id( -1 ), name( "object" ) {
    Trace::Instance().Message( "Object created.", FILENAME, LINENUMBER );
}

Object::Object( std::string Name ) : id( -1 ), name( Name ) {
    Trace::Instance().Message( "Object created.", FILENAME, LINENUMBER );
}

Object::Object( const Object& Other ) : id( -1 ), name( Other.name ) {
    Transform* transform = Other.GetComponentConst< Transform >();
    if ( transform ) {
        AddComponent( new Transform( *transform ) );
    }

    // TODO: copy components
}

void Object::Clear() {
    for ( auto component : components ) {
        delete component.second;
        component.second = nullptr;
    }
}

void Object::AddComponent( Component* Component ) {
    Component->SetParent( this );
    components.emplace( Component->GetCType(), Component );
}

void Object::SetId( int Id ) {
    Trace::Instance().Message( fmt::format( "Setting object {} with id {}.", name, Id ), FILENAME, LINENUMBER );

    id = Id;
}

int Object::GetId() const {
    return id;
}

void Object::SetName( std::string Name ) {
    name = Name;
}

std::string Object::GetName() const {
    return name;
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

Object* ObjectManager::CreateObject( std::string Name ) {
    object_list.push_back( std::make_unique< Object >( Name ) );
    Object* newObject = object_list.back().get();
    newObject->SetId( static_cast< int >( object_list.size() - 1 ) );

    return newObject;
}

Object* ObjectManager::CreateObject( std::vector< Component* > Components, std::string Name ) {
    Object* newObject = CreateObject( Name );

    for ( Component* component : Components ) {
        if ( !newObject->HasComponent( component->GetCType() ) ) {
            newObject->AddComponent( component );
        } else {
            delete component;
            component = nullptr;
        }
    }

    return newObject;
}

std::vector< std::unique_ptr< Object > >& ObjectManager::GetObjectList() {
    return object_list;
}

void ObjectManager::Print() const {
    Trace::Instance().Message( "Object List:",
                               FILENAME, LINENUMBER );

    for ( const std::unique_ptr< Object >& object : object_list ) {
        Trace::Instance().Message( fmt::format( "{}: {}", object->GetName(), object->GetId() ),
                                   FILENAME, LINENUMBER );
    }
}

ObjectManager& ObjectManager::Instance() {
    static ObjectManager objectManagerInstance;
    return objectManagerInstance;
}
