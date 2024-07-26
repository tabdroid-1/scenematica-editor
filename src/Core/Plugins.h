#pragma once

#include <Tabby.h>
#include <Core/Components.h>
#include <UI/Menus/LevelEditor.h>

struct DefaultEditorPlugin : public Tabby::Plugin {
    void Build() override
    {
        auto cameraEntity = Tabby::World::CreateEntity("Editor Camera");
        auto cc = cameraEntity.AddComponent<Tabby::CameraComponent>();
        cc.Camera.SetPerspectiveFarClip(100);

        Tabby::World::AddSystem(Tabby::Schedule::Update, [](entt::registry&) {
            TB_PROFILE_SCOPE_NAME("World::Update::EditorCameraUpdate");

            auto view = Tabby::World::GetRegistry().view<Tabby::TransformComponent, Tabby::CameraComponent, Editor::EditorCameraData>();
            for (auto entity : view) {
                auto [transform, camera] = view.get<Tabby::TransformComponent, Tabby::CameraComponent>(entity);
                auto& cameraData = view.get<Editor::EditorCameraData>(entity);

                if (Tabby::Input::IsKeyPressed(Tabby::Key::A)) {
                    transform.Translation.x -= cos(glm::radians(transform.Rotation.z)) * cameraData.Zoom * Tabby::Time::GetDeltaTime();
                    transform.Translation.y -= sin(glm::radians(transform.Rotation.z)) * cameraData.Zoom * Tabby::Time::GetDeltaTime();
                } else if (Tabby::Input::IsKeyPressed(Tabby::Key::D)) {
                    transform.Translation.x += cos(glm::radians(transform.Rotation.z)) * cameraData.Zoom * Tabby::Time::GetDeltaTime();
                    transform.Translation.y += sin(glm::radians(transform.Rotation.z)) * cameraData.Zoom * Tabby::Time::GetDeltaTime();
                }

                if (Tabby::Input::IsKeyPressed(Tabby::Key::W)) {
                    transform.Translation.x += -sin(glm::radians(transform.Rotation.z)) * cameraData.Zoom * Tabby::Time::GetDeltaTime();
                    transform.Translation.y += cos(glm::radians(transform.Rotation.z)) * cameraData.Zoom * Tabby::Time::GetDeltaTime();
                } else if (Tabby::Input::IsKeyPressed(Tabby::Key::S)) {
                    transform.Translation.x -= -sin(glm::radians(transform.Rotation.z)) * cameraData.Zoom * Tabby::Time::GetDeltaTime();
                    transform.Translation.y -= cos(glm::radians(transform.Rotation.z)) * cameraData.Zoom * Tabby::Time::GetDeltaTime();
                }

                // if (m_Rotation) {
                if (Tabby::Input::IsKeyPressed(Tabby::Key::Q))
                    transform.Rotation.z += cameraData.Zoom * Tabby::Time::GetDeltaTime();
                if (Tabby::Input::IsKeyPressed(Tabby::Key::E))
                    transform.Rotation.z -= cameraData.Zoom * Tabby::Time::GetDeltaTime();

                if (transform.Rotation.z > 180.0f)
                    transform.Rotation.z -= 360.0f;
                else if (transform.Rotation.z <= -180.0f)
                    transform.Rotation.z += 360.0f;
                // }
            }
        });

        Tabby::World::AddSystem(Tabby::Schedule::Draw, [](entt::registry&) {
            TB_PROFILE_SCOPE_NAME("World::Draw::EditorDrawMesh");

            auto view = Tabby::World::GetRegistry().view<Tabby::TransformComponent, Editor::ElementComponent>();

            for (auto entity : view) {
                auto [transform, ec] = view.get<Tabby::TransformComponent, Editor::ElementComponent>(entity);

                if (!ec.Element)
                    return;

                if (ec.Element->GetType() == Scenematica::ElementType::Brush) {

                    if (Editor::LevelEditor::IsSchematicMode()) {
                        const auto& vertices = ec.Element->GetBrushProperties().vertices;

                        for (int i = 0; i < vertices.size(); i++) {
                            TB_INFO("{}", i);
                            glm::vec3 p1 = { vertices[i].x, vertices[i].y, 0.0f };
                            glm::vec3 p2 = { vertices[0].x, vertices[0].y, 0.0f };

                            if (i < vertices.size() - 1) {
                                p2 = { vertices[i + 1].x, vertices[i + 1].y, 0.0f };
                            }

                            Tabby::Renderer2D::DrawLine(p1, p2, ec.Color);
                        }

                    } else {
                        if (ec.Mesh) {
                            ec.Mesh->SetTransform(transform.GetTransform());
                            ec.Mesh->Render();
                        }
                    }
                } else if (ec.Element->GetType() == Scenematica::ElementType::Detail) {
                } else if (ec.Element->GetType() == Scenematica::ElementType::Spawpoint) {
                }
            }
        });
    }
};
