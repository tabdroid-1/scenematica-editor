
#include <UI/Panels/ElementHierarchyPanel.h>

#include <imgui.h>

namespace Editor {

ElementHierarchyPanel::ElementHierarchyPanel(EditorLayer* layer)
    : m_EditorLayer(layer)
{
}

void ElementHierarchyPanel::OnImGuiRender()
{
    if (m_IsOpen) {
        ImGui::Begin("Scene Hierarchy", &m_IsOpen);
        ImGui::Text("Right-click to create object");

        if (ImGui::BeginPopupContextWindow("hierarchy_create_popup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            if (ImGui::MenuItem("Create element")) {
                m_SelectedElement = m_EditorLayer->m_CurrentLevel->CreateElement("New Element").GetUUID();
                m_IsSelected = true;
            }
            ImGui::EndPopup();
        }
        ImGui::Separator();

        for (auto elementGroup : m_EditorLayer->m_CurrentLevel->GetElementMap()) {
            if (!elementGroup.second.GetParent().Valid())
                RenderHierarchy(elementGroup.second.GetUUID());
        };
        ImGui::End();
    }
}

void ElementHierarchyPanel::RenderHierarchy(Scenematica::UUID elementID)
{
    Scenematica::Element& element = m_EditorLayer->m_CurrentLevel->GetElement(elementID);

    ImGuiTreeNodeFlags flags = ((m_SelectedElement == elementID) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnDoubleClick
        | ImGuiTreeNodeFlags_OpenOnArrow;

    bool node_opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)elementID, flags, element.GetName().c_str());

    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup(fmt::format("hierarchy_node_popup{}", elementID).c_str());
    }

    if (ImGui::BeginPopup(fmt::format("hierarchy_node_popup{}", elementID).c_str())) {
        if (ImGui::MenuItem("Delete")) {
            // m_Context->RemoveEntity(entity);
            m_IsSelected = false;
            m_SelectedElement = 0;
        }
        if (ImGui::MenuItem("Delete with children")) {
            // m_Context->RemoveEntityWithChildren(entity);
            m_IsSelected = false;
            m_SelectedElement = 0;
        }
        if (ImGui::MenuItem("Create child group")) {
            auto& child = m_EditorLayer->m_CurrentLevel->CreateElement("New element");
            child.SetParent(elementID);
            element.AddChildren(child.GetUUID());
            m_IsSelected = true;
            m_SelectedElement = child.GetUUID();
        }
        ImGui::EndPopup();
    }

    ImGuiDragDropFlags drag_and_drop_flags = ImGuiDragDropFlags_None;
    drag_and_drop_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect | ImGuiDragDropFlags_SourceNoPreviewTooltip;

    if (ImGui::BeginDragDropSource(drag_and_drop_flags)) {
        if (!(drag_and_drop_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
            ImGui::Text(element.GetName().c_str());
        ImGui::SetDragDropPayload("hierarchy_move_node_payload", (const void*)(uint64_t)element.GetUUID(), sizeof(element.GetUUID()));
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        ImGuiDragDropFlags target_flags = 0;
        // target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("hierarchy_move_node_payload", target_flags);
        if (payload) {
            Scenematica::UUID moved_id = *(Scenematica::UUID*)payload->Data;
            Scenematica::Element& moved_element = m_EditorLayer->m_CurrentLevel->GetElement(moved_id);

            if (moved_element.GetParent()) {
                auto previous_parent_id = moved_element.GetParent();
                auto& previous_parent = m_EditorLayer->m_CurrentLevel->GetElement(previous_parent_id);

                for (auto i = previous_parent.GetChildren().begin(); i != previous_parent.GetChildren().end(); i++) {
                    if (*i == moved_id) {
                        previous_parent.GetChildren().erase(i);
                        break;
                    }
                }
            }

            element.GetChildren().push_back(moved_id);

            moved_element.SetParent(elementID);
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::IsItemClicked()) {
        if (Tabby::Input::IsMouseButtonPressed(Tabby::Mouse::ButtonLeft)) {
            m_SelectedElement = elementID;
            m_IsSelected = true;
        }
    }
    if (node_opened) {
        for (auto child : element.GetChildren()) {
            RenderHierarchy(child);
        }
        ImGui::TreePop();
    }
}

}
