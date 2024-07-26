#include <UI/Menus/MainMenu.h>
#include <Tabby.h>
#include <EditorLayer.h>
#include <imgui.h>

namespace Editor {

MainMenu::MainMenu(EditorLayer* editorLayer)
    : MenuBase(editorLayer)
{
}

void MainMenu::OnEnter()
{
    m_EditorLayer->SetMenubarCallback([this]() {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Create New Registry")) {
                m_EditorLayer->CreateNewRegistry();
            }

            if (ImGui::MenuItem("Load Registry")) {
                m_EditorLayer->LoadRegistry();
            }

            if (ImGui::MenuItem("Exit")) {
                Tabby::Application::Get().Close();
            }
            ImGui::EndMenu();
        }
    });
}

void MainMenu::OnExit()
{
    m_EditorLayer->SetMenubarCallback([]() {
    });
}

void MainMenu::OnUpdate()
{
}

void MainMenu::OnImGuiRender()
{
    ImGui::Begin("Main Menu");

    if (ImGui::Button("Create New Registry", { 400.0f, 200.0f })) {
        m_EditorLayer->CreateNewRegistry();
    }

    if (ImGui::Button("Load Registry", { 400.0f, 200.0f })) {
        m_EditorLayer->LoadRegistry();
    }

    ImGui::End();
}

void MainMenu::OnEvent(Tabby::Event& e)
{
}
}
