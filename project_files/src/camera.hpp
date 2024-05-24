
#ifndef CAMERA_HPP
#define CAMERA_HPP
#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    bool Initialize( glm::vec3 Position );
    void Update();
    void UpdateVectors();

    glm::mat4& GetViewMatrix();

    static Camera& Instance();

    static void Movement( glm::vec3 MovementInput );

private:
    Camera();

    inline static constexpr glm::vec3 GLOBAL_UP{ 0.f, 1.f, 0.f };

    glm::mat4 view_matrix;

    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;

    glm::vec3 rotation;

    float speed = 1.f;
};

#endif
