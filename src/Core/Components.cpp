#include <Core/Components.h>

template <>
void Tabby::World::OnComponentAdded<Editor::ElementComponent>(Entity entity, Editor::ElementComponent& component)
{
    component.Color.x = Tabby::Random::Range(0.0, 1.0);
    component.Color.y = Tabby::Random::Range(0.0, 1.0);
    component.Color.z = Tabby::Random::Range(0.0, 1.0);
    component.Color.a = 1.0f;
}

template <>
void Tabby::World::OnComponentAdded<Editor::EditorCameraData>(Entity entity, Editor::EditorCameraData& component)
{
}
