#pragma once

#include <UI/Menus/MenuBase.h>

namespace Editor {

class EditorLayer;

class LevelList : public MenuBase {
public:
    LevelList(EditorLayer* editorLayer);

    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;
    void OnImGuiRender() override;
    void OnEvent(Tabby::Event& e);
};

}
