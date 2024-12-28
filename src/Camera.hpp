#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

enum class ProjectionType
{
    Perspective = 0,
    Orthographic
};

class Camera
{
public:
    Camera();

    void SetPosition(glm::vec3 const& pos);
    auto GetPosition() const -> glm::vec3 const&;

    void SetRotation(const glm::quat& rotation);
    auto GetRotation() const -> glm::quat const&;

    void SetProjectionType(ProjectionType type);
    auto GetProjectionType() const -> ProjectionType;

    void SetPerspectiveParams(float fovY, float aspect, float nearZ, float farZ);

    void SetOrthoParams(float left, float right, float bottom, float top, float nearZ, float farZ);

    auto GetViewMatrix() const -> glm::mat4;
    auto GetProjectionMatrix() const -> glm::mat4;
    auto GetViewProjectionMatrix() const -> glm::mat4;
private:
    void RecalcProjection();
private:
    ProjectionType m_projection_type = ProjectionType::Perspective;

    glm::vec3 m_position  = glm::vec3(0.f, 0.f, -4.f);
    glm::quat m_rotation  = glm::quat(1.f, 0.f, 0.f, 0.f);

    float m_fov_y   = glm::radians(45.f);
    float m_aspect = 16.f / 9.f;
    float m_near_z  = 0.1f;
    float m_far_z   = 100.f;

    float m_left   = -10.f;
    float m_right  =  10.f;
    float m_bottom = -10.f;
    float m_top    =  10.f;
    float m_far_z_ortho   = 100.f;

    glm::mat4 m_projection = glm::mat4(1.f);
};