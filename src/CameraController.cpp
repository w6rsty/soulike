#include "CameraController.hpp"


CameraController::CameraController(Camera* camera)
    : m_camera(camera)
{
}

void CameraController::Update(float deltaTime)
{
    if (!m_camera) return;

    glm::vec3 position = m_camera->GetPosition();

    glm::vec3 forwardDir(0.f, 0.f, 1.f);
    glm::vec3 rightDir (1.f, 0.f, 0.f);

    float moveDelta = m_move_speed * deltaTime;
    if (m_move_forward)  position += forwardDir * moveDelta;
    if (m_move_backward) position -= forwardDir * moveDelta;
    if (m_move_left)     position -= rightDir   * moveDelta;
    if (m_move_right)    position += rightDir   * moveDelta;

    m_camera->SetPosition(position);

    ApplyRotation();

    m_yaw   = 0.f;
    m_pitch = 0.f;
}

void CameraController::ApplyRotation()
{
    glm::quat currentRot = m_camera->GetRotation();

    glm::quat yawQ   = glm::angleAxis(glm::radians(m_yaw   * m_rotate_speed), glm::vec3(0.f, 1.f, 0.f));
    glm::quat pitchQ = glm::angleAxis(glm::radians(m_pitch * m_rotate_speed), glm::vec3(1.f, 0.f, 0.f));

    glm::quat newRot = yawQ * pitchQ * currentRot;

    m_camera->SetRotation(glm::normalize(newRot));
}