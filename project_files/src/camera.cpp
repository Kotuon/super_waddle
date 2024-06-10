
// System includes
#include <glm/gtc/matrix_transform.hpp>

// Local includes
#include "camera.hpp"
#include "input.hpp"
#include "engine.hpp"

Camera::Camera() {
}

bool Camera::Initialize( glm::vec3 Position ) {
    position = Position;
    rotation = { -90.f, -12.f, 0.f };
    orbit_radius = Position.z;

    Input::Instance().AddWASDCallback( Movement );

    UpdateVectors();

    return true;
}

void Camera::Update() {
    position = { glm::cos( glm::radians( universal_yaw_angle ) ) * orbit_radius,
                 position.y,
                 glm::sin( glm::radians( universal_yaw_angle ) ) * orbit_radius };

    rotation.x = universal_yaw_angle + 180.f;
}

void Camera::Movement( glm::vec3 MovementInput ) {

    Camera::Instance().rotation.y += -1.f * MovementInput.y * 75.f *
                                     Engine::Instance().GetDeltaTime();
    Camera::Instance().position += GLOBAL_UP * MovementInput.y * 30.f *
                                   Engine::Instance().GetDeltaTime();

    Camera::Instance().universal_yaw_angle += MovementInput.x * -1.f * 0.25f;
}

void Camera::UpdateVectors() {
    forward = glm::normalize( glm::vec3(
        glm::cos( glm::radians( rotation.x ) ) * glm::cos( glm::radians( rotation.y ) ),
        glm::sin( glm::radians( rotation.y ) ),
        glm::sin( glm::radians( rotation.x ) ) * glm::cos( glm::radians( rotation.y ) ) ) );

    right = glm::normalize( glm::cross( forward, GLOBAL_UP ) );
    up = glm::normalize( glm::cross( right, forward ) );
}

glm::mat4& Camera::GetViewMatrix() {
    UpdateVectors();

    glm::vec3 look_direction = position + forward;
    view_matrix = glm::lookAt( position, look_direction, up );

    return view_matrix;
}

float Camera::GetOrbitRadius() const {
    return orbit_radius;
}

void Camera::SetOrbitRadius( float OrbitRadius ) {
    orbit_radius = OrbitRadius;
}

Camera& Camera::Instance() {
    static Camera cameraInstance;
    return cameraInstance;
}
