#pragma once

// #include "Tabby/Renderer/OrthographicCamera.h"

#include <Tabby.h>

namespace Editor {

class CameraController {
public:
    CameraController(float aspectRatio, bool rotation = false);

    void OnUpdate();
    void OnEvent(Tabby::Event& e);

    void OnResize(float width, float height);

    Tabby::Camera& GetCamera() { return m_Camera; }
    const Tabby::Camera& GetCamera() const { return m_Camera; }

    Tabby::TransformComponent& GetTransform() { return m_CameraTransform; }
    const Tabby::TransformComponent& GetTransform() const { return m_CameraTransform; }

    float GetZoomLevel() const { return m_ZoomLevel; }
    void SetZoomLevel(float level) { m_ZoomLevel = level; }

private:
    bool OnMouseScrolled(Tabby::MouseScrolledEvent& e);
    bool OnWindowResized(Tabby::WindowResizeEvent& e);

private:
    float m_CameraSize = 10;
    float m_ZoomLevel = 1.0f;
    Tabby::Camera m_Camera;
    Tabby::TransformComponent m_CameraTransform;

    bool m_Rotation;

    float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
};

}
