#pragma once

#include <Tabby.h>

namespace Editor {

class EditorLayer;

class MenuBase {
public:
    MenuBase(EditorLayer* editorLayer)
        : m_EditorLayer(editorLayer)
    {
    }

    virtual void OnEnter() = 0;
    virtual void OnExit() = 0;

    virtual void OnUpdate() = 0;
    virtual void OnImGuiRender() = 0;
    virtual void OnEvent(Tabby::Event& e) = 0;

protected:
    EditorLayer* m_EditorLayer;
};

}
