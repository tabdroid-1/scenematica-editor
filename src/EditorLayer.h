#pragma once

#include <Tabby.h>
#include <Tabby/Audio/AudioEngine.h>
#include <scenematica/level_registry.h>
#include <scenematica/level.h>

namespace Editor {

class MenuBase;

class EditorLayer : public Tabby::Layer {
public:
    EditorLayer();

    enum EditorState {
        MainMenu = 0,
        LevelList,
        LevelEditor
    };

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(Tabby::Timestep dt) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Tabby::Event& e) override;

    void CreateNewRegistry();
    void LoadRegistry();

    void SetCurrentLevel(const std::string& levelName);
    void CreateLevel(const std::string& levelName);
    void DeleteLevel(const std::string& levelName);

    void SaveAsJson();

    void SetEditorState(EditorState state);

    void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }

    Tabby::Shared<Scenematica::Level> m_CurrentLevel;
    Tabby::Shared<Scenematica::LevelRegistry> m_CurrentLevelRegistry;

private:
    EditorState m_State;

    std::function<void()> m_MenubarCallback;

    std::unordered_map<EditorState, Tabby::Shared<Editor::MenuBase>> m_EditorMenus;
};

}
