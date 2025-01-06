#include "CameraController.hpp"
#include <glm/gtc/matrix_transform.hpp>

CameraController::CameraController(Camera* camera)
    : m_camera(camera)
{
}

void CameraController::RotateYaw(float amount)
{
    if (m_rotating)
    {
        m_yaw += amount * m_rotate_speed;
    }
}

void CameraController::RotatePitch(float amount)
{
    if (m_rotating)
    {
        m_pitch += amount * m_rotate_speed;
    }
}

void CameraController::UpdateRotation()
{
    if (m_pitch > 89.f)
    {
        m_pitch = 89.f;
    }
    if (m_pitch < -89.f)
    {
        m_pitch = -89.f;
    }

    glm::quat qyaw   = glm::angleAxis(glm::radians(m_yaw),   glm::vec3(0.f, 1.f, 0.f));
    glm::quat qpitch = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1.f, 0.f, 0.f));

    glm::quat camRot = qyaw * qpitch;

    m_camera->SetRotation(camRot);
}

void CameraController::Update(float deltaTime)
{
    if (!m_camera) return;

    UpdateRotation();

    glm::vec3 position = m_camera->GetPosition();

    glm::mat3 rotMat   = glm::mat3_cast(m_camera->GetRotation());
    glm::vec3 forward  = rotMat[2]; // dirs[2] = +Z
    glm::vec3 right    = rotMat[0]; // dirs[0] = +X
    
    float moveDelta = m_move_speed * deltaTime;
    if (m_move_forward)  position += forward * moveDelta;
    if (m_move_backward) position -= forward * moveDelta;
    if (m_move_left)     position -= right   * moveDelta;
    if (m_move_right)    position += right   * moveDelta;

    m_camera->SetPosition(position);
}
