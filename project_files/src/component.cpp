
// Local includes
#include "component.hpp"
#include "object_manager.hpp"
#include "trace.hpp"
#include "engine.hpp"

Component::Component( CType Type ) : type( Type ) {}

void Component::SetParent( Object* object ) {
    parent = object;
}

Object* Component::GetParent() const {
    return parent;
}

CType Component::GetCType() const {
    return type;
}

//---- Transform ----//

Transform::Transform() : Component( CType::CTransform ) {
}

Transform::Transform( glm::vec3 Position, glm::vec3 Scale, glm::vec3 Rotation )
    : Component( CType::CTransform ), position( Position ), old_position( 0.f ),
      scale( Scale ), rotation( Rotation ) {
}

Transform::Transform( const Transform& Other ) : Component( CType::CTransform ) {
    *this = Other;
}

void Transform::SetPosition( glm::vec3 Position ) {
    position = Position;
}

glm::vec3 Transform::GetPosition() const {
    return position;
}

void Transform::SetOldPosition( glm::vec3 OldPosition ) {
    old_position = OldPosition;
}

glm::vec3 Transform::GetOldPosition() const {
    return old_position;
}

void Transform::SetScale( glm::vec3 Scale ) {
    scale = Scale;
}

glm::vec3 Transform::GetScale() const {
    return scale;
}

void Transform::SetRotation( glm::vec3 Rotation ) {
    rotation = Rotation;
}

glm::vec3 Transform::GetRotation() const {
    return rotation;
}

CType Transform::GetCType() {
    return CType::CTransform;
}

//---- Physics ----//

Physics::Physics() : Component( CType::CPhysics ) {
}

Physics::Physics( const Physics& Other ) : Component( CType::CPhysics ) {
    *this = Other;
}

void Physics::Update() {
    if ( !GetParent() ) {
        return;
    }

    acceleration.y += GRAVITY;

    Transform* transform = GetParent()->GetComponent< Transform >();

    glm::vec3 disp = transform->GetPosition() - transform->GetOldPosition();
    transform->SetOldPosition( transform->GetPosition() );
    acceleration *= Engine::Instance().GetFixedTimeStep() * Engine::Instance().GetFixedTimeStep();
    transform->SetPosition( transform->GetPosition() + disp );
    transform->SetPosition( transform->GetPosition() + acceleration );

    acceleration = glm::vec3( 0.f );
}

void Physics::SetAcceleration( glm::vec3 Acceleration ) {
    acceleration = Acceleration;
}

glm::vec3 Physics::GetAcceleration() const {
    return acceleration;
}

void Physics::SetVelocity( glm::vec3 Velocity ) {
    velocity = Velocity;
}

glm::vec3 Physics::GetVelocity() const {
    return velocity;
}

CType Physics::GetCType() {
    return CType::CPhysics;
}
