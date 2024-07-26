#include <UI/Panels/ElementProperties.h>

#include <Tabby/Asset/AssetManager.h>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

namespace Editor {

void ElementPropertiesPanel::OnImGuiRender()
{
    ImGui::Begin("Properties");
    if (m_Selected) {
        Scenematica::Element& element = m_EditorLayer->m_CurrentLevel->GetElement(m_ElementID);

        bool isVisible = element.IsVisible();
        ImGui::Checkbox("##visible", &isVisible);
        element.SetIsVisible(isVisible);

        ImGui::SameLine();
        ImGui::TableNextColumn();
        static std::string name = element.GetName();
        ImGui::Text("Element name: ");

        ImGui::SameLine();
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputText("##", name.data(), name.capacity());
        name.resize(strlen(name.c_str()));
        element.SetName(name);

        ImGui::TableNextColumn();

        ImGui::Text("Element Type: ");

        const char* element_type_strings[] = { "Brush", "Detail", "Spawnpoint" };
        if (ImGui::BeginCombo("##element_type_combo", element_type_strings[(uint8_t)element.GetType()])) {
            for (uint8_t i = 0; i < IM_ARRAYSIZE(element_type_strings); i++) {
                bool selected = i == (uint8_t)element.GetType();
                if (ImGui::Selectable(element_type_strings[i], &selected))
                    element.SetType((Scenematica::ElementType)i);
            }
            ImGui::EndCombo();
        }

        ImGui::SeparatorText("Transform");
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 5.0f, 5.0f });
        if (ImGui::BeginTable("Transform properties", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH)) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Position");
            ImGui::SameLine();
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            Scenematica::Vector3 position = element.GetPosition();
            ImGui::DragFloat3("##floatP", position.Data(), 0.05f);
            element.SetPosition(position);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Rotation");
            ImGui::SameLine();
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            Scenematica::Vector3 rotation = element.GetRotation();
            ImGui::DragFloat3("##floatR", rotation.Data(), 0.1f);
            element.SetRotation(rotation);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Scale");
            ImGui::SameLine();
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            Scenematica::Vector3 scale = element.GetScale();
            ImGui::DragFloat3("##floatS", scale.Data(), 0.01f);
            element.SetScale(scale);

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        if (element.GetType() == Scenematica::ElementType::Brush) {

            ImGui::SeparatorText("Brush properties");
            ImGui::Text("Material: WIP");
            ImGui::Checkbox("Enable Collision", &element.GetBrushProperties().hasCollision);
            ImGui::Checkbox("Is trigger", &element.GetBrushProperties().isTrigger);
            ImGui::Checkbox("Cast shadow", &element.GetBrushProperties().castShadow);
            {

                if (ImGui::Button("Add vertex")) {
                    element.GetBrushProperties().vertices.push_back({ 0.0f, 0.0f });
                }
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                ImGui::BeginChild("Child", ImVec2(0, 260), ImGuiChildFlags_Border, window_flags);

                TB_INFO("{}", element.GetBrushProperties().vertices.size());
                for (int i = element.GetBrushProperties().vertices.size() - 1; i >= 0; --i) {
                    std::string label = std::to_string(i);
                    ImGui::DragFloat2(label.c_str(), element.GetBrushProperties().vertices[i].Data());
                    ImGui::SameLine();

                    ImGui::PushID(i); // This is required for buttons in loops to work
                    if (ImGui::Button("Remove")) {
                        element.GetBrushProperties().vertices.erase(element.GetBrushProperties().vertices.begin() + i);
                    }
                    ImGui::PopID();
                }
                ImGui::EndChild();
                ImGui::PopStyleVar();
            }

        } else if (element.GetType() == Scenematica::ElementType::Detail) {

            ImGui::SeparatorText("Detail properties");
            ImGui::Text("Material: WIP");

            static std::string detailName = element.GetDetialProperties().detailName;
            ImGui::Text("Entity name: ");

            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputText("##", detailName.data(), detailName.capacity());
            detailName.resize(strlen(detailName.c_str()));
            element.GetDetialProperties().detailName = detailName;
        } else if (element.GetType() == Scenematica::ElementType::Spawpoint) {

            ImGui::SeparatorText("Spawpoint properties");
            static std::string entityName = element.GetSpawnpointProperties().entityName;
            ImGui::Text("Entity name: ");

            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputText("##", entityName.data(), entityName.capacity());
            entityName.resize(strlen(entityName.c_str()));
            element.GetSpawnpointProperties().entityName = entityName;
        }
    }
    ImGui::End();
}

void ElementPropertiesPanel::SetElement(Scenematica::UUID elementID, bool selected)
{
    m_ElementID = elementID;
    m_Selected = selected;
}

}
