#include <Tabby/Core/Time/Timestep.h>

#include <Core/Components.h>
#include <UI/Menus/LevelEditor.h>
#include <EditorLayer.h>
#include <scenematica/level.h>
#include <imgui.h>

namespace Editor {

bool LevelEditor::SchematicMode = true;

LevelEditor::LevelEditor(EditorLayer* editorLayer)
    : MenuBase(editorLayer)
    , m_ElementHierarchyPanel(editorLayer)
    , m_ElementPropertiesPanel(editorLayer)
{

    Tabby::FramebufferSpecification fbSpec;
    fbSpec.Attachments = { Tabby::FramebufferTextureFormat::RGBA8, Tabby::FramebufferTextureFormat::RED_INTEGER, Tabby::FramebufferTextureFormat::DEPTH24STENCIL8 };
    fbSpec.Width = 2560;
    fbSpec.Height = 1600;
    m_Framebuffer = Tabby::Framebuffer::Create(fbSpec);
}

void LevelEditor::OnEnter()
{

    m_EditorLayer->SetMenubarCallback([this]() {
        if (ImGui::BeginMenu("File")) {
            // if (ImGui::MenuItem("Create New Registry")) {
            //     m_EditorLayer->CreateNewRegistry();
            // }

            // if (ImGui::MenuItem("Load Registry")) {
            //     m_EditorLayer->LoadRegistry();
            // }

            if (ImGui::MenuItem("Go to level list")) {
                TB_TRACE("State swtiched to level list");
                m_EditorLayer->SetEditorState(EditorLayer::LevelList);
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

    auto& testElement = m_EditorLayer->m_CurrentLevel->CreateElement("test");
    testElement.GetBrushProperties().vertices.push_back({ -1.0f, 1.0f });
    testElement.GetBrushProperties().vertices.push_back({ 1.0f, 1.0f });
    testElement.GetBrushProperties().vertices.push_back({ 1.0f, -1.0f });
    testElement.GetBrushProperties().vertices.push_back({ -1.0f, -1.0f });

    Tabby::Shared<Tabby::Material> tabbyMaterial;
    if (Tabby::Renderer::GetAPI() == Tabby::RendererAPI::API::OpenGL46)
        tabbyMaterial = Tabby::CreateShared<Tabby::Material>("UnlitMaterial", "shaders/gl46/Renderer3D_MeshUnlit.glsl");
    else if (Tabby::Renderer::GetAPI() == Tabby::RendererAPI::API::OpenGL33)
        tabbyMaterial = Tabby::CreateShared<Tabby::Material>("UnlitMaterial", "shaders/gl33/Renderer3D_MeshUnlit.glsl");
    else if (Tabby::Renderer::GetAPI() == Tabby::RendererAPI::API::OpenGLES3)
        tabbyMaterial = Tabby::CreateShared<Tabby::Material>("UnlitMaterial", "shaders/gles3/Renderer3D_MeshUnlit.glsl");

    for (auto& element : m_EditorLayer->m_CurrentLevel->GetElementMap()) {
        auto elementEntity = Tabby::World::CreateEntity(element.second.GetName());
        auto& ec = elementEntity.AddComponent<Editor::ElementComponent>();
        ec.Element = &element.second;

        if (ec.Element->GetType() == Scenematica::ElementType::Brush) {
            ec.Mesh = Tabby::CreateShared<Tabby::Mesh>();

            ec.Mesh->SetMaterial(tabbyMaterial);

            for (auto vertPos : element.second.GetBrushProperties().vertices) {

                Tabby::Mesh::Vertex vertex;
                vertex.Position.x = vertPos.x;
                vertex.Position.y = vertPos.y;
                ec.Mesh->AddVertex(vertex);
            }

            ec.Mesh->Create();
        }
    }
}

void LevelEditor::OnExit()
{
}

void LevelEditor::OnUpdate()
{
    Tabby::World::OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);

    if (Tabby::FramebufferSpecification spec = m_Framebuffer->GetSpecification();
        m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
        (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y)) {
        m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    }

    Tabby::Renderer2D::ResetStats();

    m_Framebuffer->Bind();
    {
        TB_PROFILE_SCOPE_NAME("Renderer Prep");

        auto clearColor = m_EditorLayer->m_CurrentLevel->GetLevelClearColor();
        Tabby::RenderCommand::SetClearColor({ clearColor.r, clearColor.g, clearColor.b, clearColor.a });
        Tabby::RenderCommand::Clear();
    }

    // This breaks renderdoc
    m_Framebuffer->ClearAttachment(1, -1);

    Tabby::World::Update(Tabby::Time::GetDeltaTime());

    auto [mx, my] = ImGui::GetMousePos();
    mx -= m_ViewportBounds[0].x;
    my -= m_ViewportBounds[0].y;
    glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
    my = viewportSize.y - my;
    int mouseX = (int)mx;
    int mouseY = (int)my;

    // TODO: Mouse picking support
    // if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y) {
    //     int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
    // m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
    // }

    m_Framebuffer->Unbind();
}

void LevelEditor::OnImGuiRender()
{
    if (!m_EditorLayer->m_CurrentLevel) {
        TB_ERROR("Current level is null");
        m_EditorLayer->SetEditorState(EditorLayer::LevelList);
    }

    auto& level = m_EditorLayer->m_CurrentLevel;

    { // Viewport
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Disable padding
        ImGui::Begin("Viewport");
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();

        Tabby::Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID(0);
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2 { m_ViewportSize.x, m_ViewportSize.y }, ImVec2 { 0, 1 }, ImVec2 { 1, 0 });

        ImGui::End();
        ImGui::PopStyleVar();
    }

    { // Panels

        m_ElementPropertiesPanel.SetElement(m_ElementHierarchyPanel.GetSelectedElement(), m_ElementHierarchyPanel.IsElementSelected());

        m_ContentBrowserPanel.OnImGuiRender();
        m_ElementHierarchyPanel.OnImGuiRender();
        m_ElementPropertiesPanel.OnImGuiRender();
    }
}

void LevelEditor::OnEvent(Tabby::Event& e)
{
}

}
