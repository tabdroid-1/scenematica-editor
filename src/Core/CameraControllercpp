#include <Core/CameraController.h>

#include <Tabby.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Editor {

CameraController::CameraController(float size, bool rotation)
    : m_CameraSize(size)
    , m_Camera()
    , m_Rotation(rotation)
{
    m_Camera.SetProjectionType(Tabby::Camera::ProjectionType::Orthographic);
    m_Camera.SetOrthographic(size, -1, 100);
}

void CameraController::OnUpdate()
{
    TB_PROFILE_SCOPE_NAME("CameraController::OnUpdate");

    if (Tabby::Input::IsKeyPressed(Tabby::Key::A)) {
        m_CameraTransform.Translation.x -= cos(glm::radians(m_CameraTransform.Rotation.z)) * m_CameraTranslationSpeed * Tabby::Time::GetDeltaTime();
        m_CameraTransform.Translation.y -= sin(glm::radians(m_CameraTransform.Rotation.z)) * m_CameraTranslationSpeed * Tabby::Time::GetDeltaTime();
    } else if (Tabby::Input::IsKeyPressed(Tabby::Key::D)) {
        m_CameraTransform.Translation.x += cos(glm::radians(m_CameraTransform.Rotation.z)) * m_CameraTranslationSpeed * Tabby::Time::GetDeltaTime();
        m_CameraTransform.Translation.y += sin(glm::radians(m_CameraTransform.Rotation.z)) * m_CameraTranslationSpeed * Tabby::Time::GetDeltaTime();
    }

    if (Tabby::Input::IsKeyPressed(Tabby::Key::W)) {
        m_CameraTransform.Translation.x += -sin(glm::radians(m_CameraTransform.Rotation.z)) * m_CameraTranslationSpeed * Tabby::Time::GetDeltaTime();
        m_CameraTransform.Translation.y += cos(glm::radians(m_CameraTransform.Rotation.z)) * m_CameraTranslationSpeed * Tabby::Time::GetDeltaTime();
    } else if (Tabby::Input::IsKeyPressed(Tabby::Key::S)) {
        m_CameraTransform.Translation.x -= -sin(glm::radians(m_CameraTransform.Rotation.z)) * m_CameraTranslationSpeed * Tabby::Time::GetDeltaTime();
        m_CameraTransform.Translation.y -= cos(glm::radians(m_CameraTransform.Rotation.z)) * m_CameraTranslationSpeed * Tabby::Time::GetDeltaTime();
    }

    if (m_Rotation) {
        if (Tabby::Input::IsKeyPressed(Tabby::Key::Q))
            m_CameraTransform.Rotation.z += m_CameraRotationSpeed * Tabby::Time::GetDeltaTime();
        if (Tabby::Input::IsKeyPressed(Tabby::Key::E))
            m_CameraTransform.Rotation.z -= m_CameraRotationSpeed * Tabby::Time::GetDeltaTime();

        if (m_CameraTransform.Rotation.z > 180.0f)
            m_CameraTransform.Rotation.z -= 360.0f;
        else if (m_CameraTransform.Rotation.z <= -180.0f)
            m_CameraTransform.Rotation.z += 360.0f;
    }

    m_CameraTranslationSpeed = m_ZoomLevel;

    glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians((glm::vec3&)m_CameraTransform.Rotation)));
    m_CameraTransform.TransformMatrix = glm::translate(glm::mat4(1.0f), (glm::vec3&)m_CameraTransform.Translation) * rotation * glm::scale(glm::mat4(1.0f), (glm::vec3&)m_CameraTransform.Scale);
}

void CameraController::OnEvent(Tabby::Event& e)
{
    TB_PROFILE_SCOPE_NAME("CameraController::OnEvent(Event)");

    Tabby::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Tabby::MouseScrolledEvent>(TB_BIND_EVENT_FN(CameraController::OnMouseScrolled));
    dispatcher.Dispatch<Tabby::WindowResizeEvent>(TB_BIND_EVENT_FN(CameraController::OnWindowResized));
}

void CameraController::OnResize(float width, float height)
{
    m_Camera.SetViewportSize(width, height);
}

bool CameraController::OnMouseScrolled(Tabby::MouseScrolledEvent& e)
{
    TB_PROFILE_SCOPE_NAME("CameraController::OnEvent(MouseScrolledEvent)");

    m_ZoomLevel -= e.GetYOffset() * 0.25f;
    m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
    m_Camera.SetOrthographic(m_CameraSize * m_ZoomLevel, -1, 25);
    return false;
}

bool CameraController::OnWindowResized(Tabby::WindowResizeEvent& e)
{
    TB_PROFILE_SCOPE_NAME("CameraController::OnEvent(WindowResizeEvent)");

    OnResize((float)e.GetWidth(), (float)e.GetHeight());
    return false;
}

}
