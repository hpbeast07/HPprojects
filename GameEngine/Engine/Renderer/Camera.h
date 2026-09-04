#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// Camera.h — reusable perspective camera and editor-style fly controller
// ─────────────────────────────────────────────────────────────────────────────
#include "../Math/Math.h"

namespace GE {

class PerspectiveCamera {
public:
    PerspectiveCamera(float verticalFovDegrees = 60.0f,
                      float aspectRatio = 16.0f / 9.0f,
                      float nearClip = 0.1f,
                      float farClip = 1000.0f);

    void SetPerspective(float verticalFovDegrees, float aspectRatio,
                        float nearClip, float farClip);
    void SetViewportSize(uint32_t width, uint32_t height);

    void SetPosition(const Vec3& position);
    void SetYawPitch(float yawDegrees, float pitchDegrees);
    void LookAt(const Vec3& target, const Vec3& worldUp = Vec3(0.0f, 1.0f, 0.0f));

    const Vec3& GetPosition() const { return m_Position; }
    const Vec3& GetForward() const { return m_Forward; }
    const Vec3& GetRight() const { return m_Right; }
    const Vec3& GetUp() const { return m_Up; }
    float GetYaw() const { return m_Yaw; }
    float GetPitch() const { return m_Pitch; }
    float GetFov() const { return m_VerticalFov; }
    float GetNearClip() const { return m_NearClip; }
    float GetFarClip() const { return m_FarClip; }

    const Mat4& GetViewMatrix() const { return m_View; }
    const Mat4& GetProjectionMatrix() const { return m_Projection; }
    Mat4 GetViewProjectionMatrix() const { return m_Projection * m_View; }

private:
    void RecalculateProjection();
    void RecalculateView();

    Vec3 m_Position{ 0.0f, 0.0f, 3.0f };
    Vec3 m_Forward{ 0.0f, 0.0f, -1.0f };
    Vec3 m_Right{ 1.0f, 0.0f, 0.0f };
    Vec3 m_Up{ 0.0f, 1.0f, 0.0f };
    Vec3 m_WorldUp{ 0.0f, 1.0f, 0.0f };

    float m_Yaw = -90.0f;
    float m_Pitch = 0.0f;
    float m_VerticalFov = 60.0f;
    float m_AspectRatio = 16.0f / 9.0f;
    float m_NearClip = 0.1f;
    float m_FarClip = 1000.0f;

    Mat4 m_View{ 1.0f };
    Mat4 m_Projection{ 1.0f };
};

class FlyCameraController {
public:
    explicit FlyCameraController(PerspectiveCamera& camera);

    void OnUpdate(float deltaTime, bool allowInput = true);
    void SetMoveSpeed(float speed) { m_MoveSpeed = glm::max(speed, 0.0f); }
    void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = glm::max(sensitivity, 0.0f); }
    void SetScrollSensitivity(float sensitivity) { m_ScrollSensitivity = glm::max(sensitivity, 0.0f); }

    float GetMoveSpeed() const { return m_MoveSpeed; }
    float GetMouseSensitivity() const { return m_MouseSensitivity; }
    bool IsLooking() const { return m_Looking; }

private:
    PerspectiveCamera& m_Camera;
    float m_MoveSpeed = 5.0f;
    float m_MouseSensitivity = 0.12f;
    float m_ScrollSensitivity = 1.5f;
    bool m_Looking = false;
};

} // namespace GE