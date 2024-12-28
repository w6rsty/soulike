#pragma once
#include "Camera.hpp"

class CameraController
{
public:
    CameraController(Camera* camera);

    void Update(float deltaTime);

    void SetMoveSpeed(float speed)   { m_move_speed = speed; }
    void SetRotateSpeed(float speed) { m_rotate_speed = speed; }

    void MoveForward(bool enable)    { m_move_forward = enable; }
    void MoveBackward(bool enable)   { m_move_backward = enable; }
    void MoveLeft(bool enable)       { m_move_left = enable; }
    void MoveRight(bool enable)      { m_move_right = enable; }
    void EnableRotating(bool enable) { m_rotating = enable; }
    void RotateYaw(float amount)
    {
        if (m_rotating)
        {
            m_yaw += amount;
        }
    }
    void RotatePitch(float amount)
    {
        if (m_rotating)
        {
            m_pitch += amount;
        }
    }
private:
    void ApplyRotation();
private:
    Camera* m_camera = nullptr;

    float m_move_speed   = 5.0f;
    float m_rotate_speed = 1.0f;

    bool m_move_forward  = false;
    bool m_move_backward = false;
    bool m_move_left     = false;
    bool m_move_right    = false;
    bool m_rotating      = false;

    float m_yaw   = 0.f;
    float m_pitch = 0.f;
};
