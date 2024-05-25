
#ifndef COMPONENT_HPP
#define COMPONENT_HPP
#pragma once

// System includes
#include <glm/glm.hpp>

class Object;

class Component {
public:
    enum CType {
        CTransform,
        CPhysics,
        CCollision,
        CModel,
    };

    Component( CType Type );

    void SetParent( Object* Object );
    Object* GetParent() const;
    CType GetCType() const;

private:
    CType type;
    Object* parent;
};

typedef Component::CType CType;

class Transform : public Component {
public:
    Transform();
    Transform( glm::vec3 Position, glm::vec3 Scale, glm::vec3 Rotation );
    Transform( const Transform& Other );

    void SetPosition( glm::vec3 Position );
    glm::vec3 GetPosition() const;

    void SetOldPosition( glm::vec3 OldPosition );
    glm::vec3 GetOldPosition() const;

    void SetScale( glm::vec3 Scale );
    glm::vec3 GetScale() const;

    void SetRotation( glm::vec3 Rotation );
    glm::vec3 GetRotation() const;

    static CType GetCType();

private:
    glm::vec3 position{ 1.f, 1.f, 1.f };
    glm::vec3 old_position{ 1.f, 1.f, 1.f };
    glm::vec3 scale{ 1.f, 1.f, 1.f };
    glm::vec3 rotation{ 1.f, 1.f, 1.f };
};

constexpr float GRAVITY = -15.0f;

class Physics : public Component {
public:
    Physics();
    Physics( const Physics& Other );

    void Update();

    void SetAcceleration( glm::vec3 Acceleration );
    glm::vec3 GetAcceleration() const;

    void SetVelocity( glm::vec3 Velocity );
    glm::vec3 GetVelocity() const;

    static CType GetCType();

private:
    glm::vec3 acceleration;
    glm::vec3 velocity;
};

class Collision : public Component {
public:
private:
};

#endif
