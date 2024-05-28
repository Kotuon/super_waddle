
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
    rotation = { -90.f, 0.f, 0.f };
    orbit_radius = Position.z;

    Input::Instance().AddWASDCallback( Movement );

    UpdateVectors();

    return true;
}

void Camera::Update() {
    float universalAngle = Engine::Instance().GetTotalTime() * 10.f;
    position = { glm::cos( glm::radians( universalAngle ) ) * orbit_radius,
                 position.y,
                 glm::sin( glm::radians( universalAngle ) ) * orbit_radius };

    rotation.x = universalAngle + 180.f;
}

void Camera::Movement( glm::vec3 MovementInput ) {
    Camera::Instance().position += Camera::Instance().up * MovementInput.y * 50.f *
                                   Engine::Instance().GetDeltaTime();

    Camera::Instance().rotation.y += -1.f * MovementInput.y * 75.f * Engine::Instance().GetDeltaTime();
    Camera::Instance().SetOrbitRadius( Camera::Instance().GetOrbitRadius() +
                                       ( -1.f * MovementInput.y * 2.f * Engine::Instance().GetDeltaTime() ) );
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
