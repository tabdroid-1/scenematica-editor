#include <EditorLayer.h>
#include <UI/Menus/MenuBase.h>
#include <UI/Menus/MainMenu.h>
#include <UI/Menus/LevelList.h>
#include <UI/Menus/LevelEditor.h>
#include <Core/Plugins.h>
#include <imgui.h>

namespace Editor {

EditorLayer::EditorLayer()
    : m_State(MainMenu)
{
    m_EditorMenus[MainMenu] = Tabby::CreateShared<Editor::MainMenu>(this);
    m_EditorMenus[LevelList] = Tabby::CreateShared<Editor::LevelList>(this);
    m_EditorMenus[LevelEditor] = Tabby::CreateShared<Editor::LevelEditor>(this);
}

void EditorLayer::OnAttach()
{
    TB_PROFILE_SCOPE();

    Tabby::World::Init();
    Tabby::World::OnStart();

    Tabby::World::AddPlugins<DefaultEditorPlugin>();
}

void EditorLayer::OnDetach()
{
    TB_PROFILE_SCOPE();

    Tabby::World::OnStop();
}

void EditorLayer::OnUpdate(Tabby::Timestep dt)
{
    m_EditorMenus[m_State]->OnUpdate();
}

void EditorLayer::OnImGuiRender()
{
    TB_PROFILE_SCOPE_NAME("EditorLayer::OnImGuiRender()");

    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    if (m_MenubarCallback)
        window_flags |= ImGuiWindowFlags_MenuBar;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    // window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("OpenGLAppDockspace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (m_MenubarCallback) {
        if (ImGui::BeginMenuBar()) {
            m_MenubarCallback();
            ImGui::EndMenuBar();
        }
    }

    {
        m_EditorMenus[m_State]->OnImGuiRender();
    }
    ImGui::End();
}

void EditorLayer::OnEvent(Tabby::Event& e)
{
    m_EditorMenus[m_State]->OnEvent(e);
}

void EditorLayer::SetEditorState(EditorState state)
{

    TB_PROFILE_SCOPE_NAME("EditorLayer::SetEditorState()");
    if (state != m_State) {
        m_EditorMenus[m_State]->OnExit();
        m_State = state;
        m_EditorMenus[m_State]->OnEnter();
    }
}

void EditorLayer::LoadRegistry()
{
    TB_PROFILE_SCOPE_NAME("EditorLayer::LoadRegistry()");
    std::string path = Tabby::FileDialogs::OpenFile(".smlr");

    if (path != "") {

        m_CurrentLevelRegistry = Tabby::CreateShared<Scenematica::LevelRegistry>(path);
        TB_TRACE("Loaded Registry");
        SetEditorState(EditorLayer::LevelList);
    } else {

        TB_WARN("File dialog closed or path not found");
    }
}

void EditorLayer::CreateNewRegistry()
{
    TB_PROFILE_SCOPE_NAME("EditorLayer::CreateNewRegistry()");
    m_CurrentLevelRegistry = Tabby::CreateShared<Scenematica::LevelRegistry>();
    TB_TRACE("Created New Registry");
    SetEditorState(EditorLayer::LevelList);
}

void EditorLayer::SetCurrentLevel(const std::string& levelName)
{
    TB_PROFILE_SCOPE_NAME("EditorLayer::SetCurrentLevel");
    auto& levels = m_CurrentLevelRegistry->GetLevelMap();
    if (levels.find(levelName) != levels.end()) {
        m_CurrentLevel = levels[levelName];
        TB_TRACE("Set current level: {0}", levelName);
    }
}

void EditorLayer::CreateLevel(const std::string& levelName)
{
    TB_PROFILE_SCOPE_NAME("EditorLayer::SetCurrentLevel");
    auto& levels = m_CurrentLevelRegistry->GetLevelMap();
    if (levels.find(levelName) == levels.end()) {

        levels[levelName] = Tabby::CreateShared<Scenematica::Level>(levelName);
        TB_TRACE("Created level {0}", levelName);
    }
}

void EditorLayer::DeleteLevel(const std::string& levelName)
{
    auto& levels = m_CurrentLevelRegistry->GetLevelMap();
    if (levels.find(levelName) != levels.end()) {
        levels.erase(levelName);
        TB_TRACE("Deleted level \"{0}\"", levelName);
    }
}

void EditorLayer::SaveAsJson()
{
    std::string path = Tabby::FileDialogs::SaveFile(".smlrj");

    if (path == "") {
        TB_ERROR("File dialog closed or path not found");
        return;
    }

    Scenematica::LevelRegistry::SerializeToJson(*m_CurrentLevelRegistry, path);
    TB_TRACE("Saved registry \"{0}\"", m_CurrentLevelRegistry->GetRegistryName());
}

}
