#include <UI/Menus/LevelList.h>
#include <EditorLayer.h>
#include <scenematica/level.h>
#include <imgui.h>
#include <Tabby.h>
#include <Tabby/Core/Filesystem/Filesystem.h>

namespace Editor {

LevelList::LevelList(EditorLayer* editorLayer)
    : MenuBase(editorLayer)
{
}

void LevelList::OnEnter()
{
    m_EditorLayer->SetMenubarCallback([this]() {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Create New Registry")) {
                m_EditorLayer->CreateNewRegistry();
            }

            if (ImGui::MenuItem("Load Registry")) {
                m_EditorLayer->LoadRegistry();
            }

            if (ImGui::MenuItem("Save As Json")) {
                m_EditorLayer->SaveAsJson();
            }

            if (ImGui::MenuItem("Exit")) {
                Tabby::Application::Get().Close();
            }
            ImGui::EndMenu();
        }
    });
}

void LevelList::OnExit()
{
}

void LevelList::OnUpdate()
{
}

void LevelList::OnImGuiRender()
{

    if (!m_EditorLayer->m_CurrentLevelRegistry) {
        TB_ERROR("Level registry is null! Switching to main menu.");
        m_EditorLayer->SetEditorState(EditorLayer::MainMenu);
        return;
    }

    {

        ImGui::Begin("Level List");

        auto& levels = m_EditorLayer->m_CurrentLevelRegistry->GetLevelMap();
        static std::string current_selected_level;

        {
            TB_PROFILE_SCOPE_NAME("LevelList::OnImGuiRender::LevelList");
            if (ImGui::BeginListBox("Level List", ImVec2(ImGui::GetWindowWidth(), 5 * ImGui::GetTextLineHeightWithSpacing()))) {
                for (auto& level : levels) {
                    const bool is_selected = (current_selected_level == level.first);
                    if (ImGui::Selectable(level.first.c_str(), is_selected))
                        current_selected_level = level.first;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }
        }

        static std::string levelToBeAdded = "Level";

        {
            if (ImGui::BeginPopupContextItem("create_level")) {
                ImGui::Text("Level Name: ");
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::InputText("##", levelToBeAdded.data(), 25);
                levelToBeAdded.resize(strlen(levelToBeAdded.c_str()));

                if (ImGui::Button("Add")) {
                    m_EditorLayer->CreateLevel(levelToBeAdded);
                    ImGui::CloseCurrentPopup();
                    levelToBeAdded = "Level";
                }

                ImGui::EndPopup();
            }
        }

        if (ImGui::Button("Create New Level"))
            ImGui::OpenPopup("create_level");
        ImGui::SameLine();

        {
            TB_PROFILE_SCOPE_NAME("LevelList::OnImGuiRender::LoadLevel");
            if (current_selected_level != "") {
                std::string buf = "Load " + current_selected_level;
                if (ImGui::Button(buf.c_str())) {
                    m_EditorLayer->SetCurrentLevel(current_selected_level);

                    if (m_EditorLayer->m_CurrentLevel) {
                        m_EditorLayer->SetEditorState(EditorLayer::LevelEditor);

                        ImGui::CloseCurrentPopup();
                    }
                }
            }
        }

        ImGui::End();
    }

    {

        ImGui::Begin("Registry Properties");
        std::string& registryName = m_EditorLayer->m_CurrentLevelRegistry->GetRegistryName();

        ImGui::TableNextColumn();
        ImGui::Text("Level registry name: ");

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputText("##", registryName.data(), 25);
        registryName.resize(strlen(registryName.c_str()));

        // ImGui::Text("Level registry name: %s", registryName.c_str());

        ImGui::End();
    }
}

void LevelList::OnEvent(Tabby::Event& e)
{
}

}
