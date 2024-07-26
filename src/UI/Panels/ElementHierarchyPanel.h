#pragma once

#include <scenematica/elements.h>
#include <EditorLayer.h>

namespace Editor {

class ElementHierarchyPanel {
public:
    ElementHierarchyPanel(EditorLayer* layer);

    void OnImGuiRender();
    void SetSelectedID(Scenematica::UUID elementID, bool is_selected)
    {
        m_SelectedElement = elementID;
        m_IsSelected = is_selected;
    }

    Scenematica::UUID GetSelectedElement() const { return m_SelectedElement; }
    bool IsElementSelected() const { return m_IsSelected; }

private:
    void RenderHierarchy(Scenematica::UUID elementID);

private:
    Scenematica::UUID m_SelectedElement;
    bool m_IsSelected = false;

    bool m_IsOpen = true;

    EditorLayer* m_EditorLayer;
};

}
