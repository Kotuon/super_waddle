
#ifndef OBJECT_MANAGER_HPP
#define OBJECT_MANAGER_HPP
#pragma once

// std includes
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

// Local includes
#include "component.hpp"

class Object {
public:
    Object();
    Object( std::string Name );
    Object( const Object& other );

    void Clear();

    void AddComponent( Component* Component );

    void SetId( int Id );
    int GetId() const;

    void SetName( std::string Name );
    std::string GetName() const;

    void Update();

    bool HasComponent( CType Type );

    template < typename T >
    T* GetComponent() {
        // Searching for component using the type (enum as int)
        auto found = components.find( T::GetCType() );
        if ( found == components.end() ) {
            return nullptr;
        }
        // Cast found component into correct type
        return reinterpret_cast< T* >( found->second );
    }

    template < typename T >
    void RemoveComponent() {
        // Searching for component using the type (enum as int)
        auto found = components.find( T::GetCType() );
        if ( found == components.end() )
            return;
        // Delete component
        delete found->second;
        found->second = nullptr;
        // Remove pointer from map
        components.erase( found->first );
    }

private:
    template < typename T >
    T* GetComponentConst() const {
        // Searching for component using the type (enum as int)
        auto found = components.find( T::GetCType() );
        if ( found == components.end() ) {
            return nullptr;
        }
        // Cast found component into correct type
        return reinterpret_cast< T* >( found->second );
    }

    std::unordered_map< CType, Component* > components;
    std::string name;
    int id;
};

class ObjectManager {
public:
    Object* CreateObject( std::string Name );
    Object* CreateObject( std::vector< Component* > Components, std::string Name );

    std::vector< std::unique_ptr< Object > >& GetObjectList();

    void DestoryObject( Object* ToDestory );

    void Print() const;

    static ObjectManager& Instance();

private:
    ObjectManager();

    std::vector< std::unique_ptr< Object > > object_list;
};

#endif
