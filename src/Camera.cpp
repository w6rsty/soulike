#define GLM_ENABLE_EXPERIMENTAL
#include "Camera.hpp"

Camera::Camera()
{
    RecalcProjection();
}

void Camera::SetPosition(glm::vec3 const& pos)
{
    m_position = pos;
}

auto Camera::GetPosition() const -> glm::vec3 const&
{
    return m_position;
}

void Camera::SetRotation(const glm::quat& rotation)
{
    m_rotation = rotation;
}

auto Camera::GetRotation() const -> glm::quat const&
{
    return m_rotation;
}

void Camera::SetProjectionType(ProjectionType type)
{
    m_projection_type = type;
    RecalcProjection();
}

auto Camera::GetProjectionType() const -> ProjectionType
{
    return m_projection_type;
}

void Camera::SetPerspectiveParams(float fovY, float aspect, float nearZ, float farZ)
{
    m_fov_y   = fovY;
    m_aspect = aspect;
    m_near_z  = nearZ;
    m_far_z   = farZ;
    if (m_projection_type == ProjectionType::Perspective)
    {
        RecalcProjection();
    }
}

void Camera::SetOrthoParams(float left, float right, float bottom, float top, float nearZ, float farZ)
{
    m_ortho_left  = left;
    m_ortho_right = right;
    m_ortho_bottom = bottom;
    m_ortho_top    = top;
    m_near_z  = nearZ;
    m_far_z   = farZ;
    if (m_projection_type == ProjectionType::Orthographic)
    {
        RecalcProjection();
    }
}

void Camera::RecalcProjection()
{
    switch (m_projection_type)
    {
    case ProjectionType::Perspective:
        {
            m_projection = glm::perspectiveLH_ZO(m_fov_y, m_aspect, m_near_z, m_far_z);
            break;
        }
    case ProjectionType::Orthographic:
        {
            m_projection = glm::orthoLH_ZO(m_ortho_left, m_ortho_right, m_ortho_bottom, m_ortho_top, m_near_z, m_far_z);
            break;
        }
    }
}

auto Camera::GetViewMatrix() const -> glm::mat4
{
    glm::mat4 rotationMat = glm::mat4_cast(m_rotation);
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), -m_position);
    glm::mat4 view = glm::mat4_cast(glm::conjugate(m_rotation)) * translationMat;

    return view;
}

auto Camera::GetProjectionMatrix() const -> glm::mat4
{
    return m_projection;
}
auto Camera::GetViewProjectionMatrix() const -> glm::mat4
{
    return GetProjectionMatrix() * GetViewMatrix();
}
