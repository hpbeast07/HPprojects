#include "Camera.h"
#include "../Input/Input.h"

#include <algorithm>
#include <cmath>

namespace GE {

PerspectiveCamera::PerspectiveCamera(float verticalFovDegrees, float aspectRatio,
                                     float nearClip, float farClip) {
    SetPerspective(verticalFovDegrees, aspectRatio, nearClip, farClip);
    RecalculateView();
}

void PerspectiveCamera::SetPerspective(float verticalFovDegrees, float aspectRatio,
                                       float nearClip, float farClip) {
    m_VerticalFov = glm::clamp(verticalFovDegrees, 1.0f, 179.0f);
    m_AspectRatio = glm::max(aspectRatio, 0.001f);
    m_NearClip = glm::max(nearClip, 0.001f);
    m_FarClip = glm::max(farClip, m_NearClip + 0.001f);
    RecalculateProjection();
}

void PerspectiveCamera::SetViewportSize(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0) return;
    m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
    RecalculateProjection();
}

void PerspectiveCamera::SetPosition(const Vec3& position) {
    m_Position = position;
    RecalculateView();
}

void PerspectiveCamera::SetYawPitch(float yawDegrees, float pitchDegrees) {
    m_Yaw = yawDegrees;
    m_Pitch = glm::clamp(pitchDegrees, -89.0f, 89.0f);
    RecalculateView();
}

void PerspectiveCamera::LookAt(const Vec3& target, const Vec3& worldUp) {
    const Vec3 direction = target - m_Position;
    if (glm::length2(direction) <= EPSILON) return;

    m_WorldUp = glm::normalize(worldUp);
    m_Forward = glm::normalize(direction);
    m_Pitch = Degrees(std::asin(glm::clamp(m_Forward.y, -1.0f, 1.0f)));
    m_Yaw = Degrees(std::atan2(m_Forward.z, m_Forward.x));
    RecalculateView();
}

void PerspectiveCamera::RecalculateProjection() {
    m_Projection = glm::perspective(Radians(m_VerticalFov), m_AspectRatio,
                                    m_NearClip, m_FarClip);
}

void PerspectiveCamera::RecalculateView() {
    const float yawRadians = Radians(m_Yaw);
    const float pitchRadians = Radians(m_Pitch);
    m_Forward = glm::normalize(Vec3(
        std::cos(yawRadians) * std::cos(pitchRadians),
        std::sin(pitchRadians),
        std::sin(yawRadians) * std::cos(pitchRadians)));

    m_Right = glm::normalize(glm::cross(m_Forward, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
    m_View = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
}

FlyCameraController::FlyCameraController(PerspectiveCamera& camera)
    : m_Camera(camera) {}

void FlyCameraController::OnUpdate(float deltaTime, bool allowInput) {
    if (!allowInput) {
        if (m_Looking) {
            Input::SetCursorLocked(false);
            m_Looking = false;
        }
        return;
    }

    const bool wantsLook = Input::IsMouseButtonDown(MouseButton::Right);
    if (wantsLook != m_Looking) {
        m_Looking = wantsLook;
        Input::SetCursorLocked(m_Looking);
    }

    Vec3 movement(0.0f);
    if (Input::IsKeyDown(KeyCode::W)) movement += m_Camera.GetForward();
    if (Input::IsKeyDown(KeyCode::S)) movement -= m_Camera.GetForward();
    if (Input::IsKeyDown(KeyCode::D)) movement += m_Camera.GetRight();
    if (Input::IsKeyDown(KeyCode::A)) movement -= m_Camera.GetRight();
    if (Input::IsKeyDown(KeyCode::E)) movement += Vec3(0.0f, 1.0f, 0.0f);
    if (Input::IsKeyDown(KeyCode::Q)) movement -= Vec3(0.0f, 1.0f, 0.0f);

    float speed = m_MoveSpeed;
    if (Input::IsKeyDown(KeyCode::LeftShift)) speed *= 3.0f;
    if (Input::IsKeyDown(KeyCode::LeftControl)) speed *= 0.25f;
    if (glm::length2(movement) > EPSILON)
        m_Camera.SetPosition(m_Camera.GetPosition() + glm::normalize(movement) * speed * deltaTime);

    const float scroll = Input::GetScrollDelta();
    if (scroll != 0.0f)
        m_MoveSpeed = glm::clamp(m_MoveSpeed + scroll * m_ScrollSensitivity, 0.25f, 100.0f);

    if (!m_Looking) return;

    const Vec2 mouseDelta = Input::GetMouseDelta();
    if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) {
        m_Camera.SetYawPitch(m_Camera.GetYaw() + mouseDelta.x * m_MouseSensitivity,
                             m_Camera.GetPitch() - mouseDelta.y * m_MouseSensitivity);
    }
}

} // namespace GE