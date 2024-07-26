#pragma once

#include <EditorLayer.h>
#include <scenematica.h>

namespace Editor {

class ElementPropertiesPanel {
public:
    ElementPropertiesPanel(EditorLayer* editorLayer)
        : m_EditorLayer(editorLayer) {
        };

    void OnImGuiRender();

    void SetElement(Scenematica::UUID elementID, bool selected);

private:
    EditorLayer* m_EditorLayer;

    Scenematica::UUID m_ElementID;
    bool m_Selected = false;
};

}
