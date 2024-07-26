#include "ContentBrowser.h"

#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Asset/AssetType.h>
#include <Tabby/Core/Filesystem/Filesystem.h>

#include <imgui.h>

namespace Editor {

const char* AssetTypeToIcon(Tabby::AssetType type)
{
    switch (type) {
    case Tabby::AssetType::MESH_SRC:
        return "3d-modeling.png";
    case Tabby::AssetType::MATERIAL:
        return "document.png";
    case Tabby::AssetType::AUDIO_SRC:
        return "document.png";
    case Tabby::AssetType::IMAGE_SRC:
        return "image-gallery.png";
    case Tabby::AssetType::TABBY_IMAGE:
        return "image-gallery.png";
    case Tabby::AssetType::TABBY_MESH:
        return "3d_modeling.png";
    case Tabby::AssetType::UNKNOWN:
        return "document.png";
    default:
        return "document.png";
    }
}

ContentBrowser::ContentBrowser()
{
    m_WorkingDirectory = Tabby::FileSystem::GetWorkingDirectory();
    m_CurrentDirectory = m_WorkingDirectory;

    std::filesystem::directory_iterator i("icons/content_browser");

    for (auto& entry : i) {
        Tabby::AssetHandle image = Tabby::AssetManager::Get()->LoadAssetSource(entry.path());

        m_IconMap.emplace(entry.path().filename().string(), Tabby::AssetManager::Get()->GetAsset<Tabby::Texture>(image));
    }
}

ContentBrowser::~ContentBrowser()
{
    for (auto& icon : m_IconMap) {
        icon.second->Destroy();
    }
}

void ContentBrowser::OnImGuiRender()
{
    auto texture_registry = Tabby::AssetManager::Get()->GetAssetRegistry();

    ImGui::Begin("Content browser");

    if (ImGui::BeginPopupContextItem("create_level")) {
        // ImGui::Begin("Create directory", &m_CreateDirectoryWindowActive, ImGuiWindowFlags_NoDocking);

        ImGui::Text("Directory name: ");

        ImGui::SameLine();
        static std::string text;
        ImGui::InputText("##cb_create_dir_window", text.data(), 100);
        text.resize(strlen(text.c_str()));

        if (ImGui::Button("Create")) {
            m_CreateDirectoryWindowActive = false;
            std::filesystem::create_directory(m_CurrentDirectory.string() + "/" + text);
            FetchCurrentDirectory();
            text.clear();
        }

        ImGui::SameLine();
        if (ImGui::Button("Close")) {
            m_CreateDirectoryWindowActive = false;
            text.clear();
        }

        // ImGui::End();

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupContextWindow("##cb_create_directory_popup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {

        if (ImGui::MenuItem("Create directory")) {
            ImGui::OpenPopup("create_level");
        }
        ImGui::EndPopup();
    }

    ImGui::BeginDisabled(m_CurrentDirectory == m_WorkingDirectory);
    // if (Tabby::UI::RenderImageButton(m_IconMap["btn_back.png"], m_Context->GetRenderer()->GetSamplerLinear(), { 18, 18 })) {
    //     m_CurrentDirectory = m_CurrentDirectory.parent_path();
    //     FetchCurrentDirectory();
    // }

    if (ImGui::ImageButton((ImTextureID)m_IconMap["btn_back.png"]->GetRendererID(), { 18, 18 }, { 0, 1 }, { 1, 0 })) {
        m_CurrentDirectory = m_CurrentDirectory.parent_path();
        FetchCurrentDirectory();
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    auto relative = std::filesystem::relative(m_CurrentDirectory, m_WorkingDirectory).string();
    std::replace(relative.begin(), relative.end(), '\\', '/');

    // HACK
    if (relative.length() == 1 && relative[0] == '.')
        relative = "Project root";

    ImGui::SetCursorPos({ ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5 });
    ImGui::Text(relative.c_str());

    ImGui::Separator();

    if (m_DirectoryFetchTimer.ElapsedMillis() > 100) {
        FetchCurrentDirectory();
        m_DirectoryFetchTimer.Reset();
    }

    const float padding = 16.0f;
    const float thumbnail_size = 96.0f;
    const float cell_size = thumbnail_size + padding;

    float cb_panel_width = ImGui::GetContentRegionAvail().x;
    uint32_t column_count = std::clamp((uint32_t)cb_panel_width / (uint32_t)cell_size, 1u, 15u);

    if (ImGui::BeginTable("##cb_content_table", column_count)) {
        ImGui::TableNextRow();
        uint32_t item_id = column_count;
        bool filesystem_refetch_requested = false;

        for (auto& entry : m_CurrentDirectoryEntries) {
            if (!(item_id % column_count))
                ImGui::TableNextRow();

            ImGui::TableNextColumn();

            ImGui::PushID(fmt::format("cb_item_{}", entry.string()).c_str());

            if (std::filesystem::is_directory(entry)) {
                ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
                ImGui::ImageButton((ImTextureID)m_IconMap["folder.png"]->GetRendererID(), { 36, 36 }, { 0, 1 }, { 1, 0 });
                ImGui::PopStyleColor();
            } else if (entry.extension() == ".txt") {
                ImGui::ImageButton((ImTextureID)m_IconMap["document.png"]->GetRendererID(), { 36, 36 }, { 0, 1 }, { 1, 0 });
            } else {
                ImGui::ImageButton((ImTextureID)m_IconMap[AssetTypeToIcon(Tabby::FileExtensionToAssetType(entry.extension().string()))]->GetRendererID(), { 36, 36 }, { 0, 1 }, { 1, 0 });
            }

            ImGuiDragDropFlags drag_and_drop_flags = ImGuiDragDropFlags_None;
            drag_and_drop_flags |= ImGuiDragDropFlags_SourceAllowNullID;

            if (ImGui::BeginDragDropSource(drag_and_drop_flags)) {
                if (!(drag_and_drop_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
                    ImGui::Text(entry.string().c_str());
                ImGui::SetDragDropPayload("content_browser_item", entry.string().c_str(), entry.string().size());
                ImGui::EndDragDropSource();
            }

            ImGui::PopID();

            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
                ImGui::OpenPopup(fmt::format("##cb_item_actions_{}", entry.string()).c_str());
            }

            else if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && std::filesystem::is_directory(entry)) {
                m_CurrentDirectory /= entry.filename();
                filesystem_refetch_requested = true;
            }

            if (ImGui::BeginPopup(fmt::format("##cb_item_actions_{}", entry.string()).c_str())) {
                if (ImGui::MenuItem("Delete")) {
                    std::filesystem::remove_all(entry);
                    filesystem_refetch_requested = true;
                }
                if ((uint8_t)Tabby::FileExtensionToAssetType(entry.extension().string()) < 4) {
                    if (ImGui::MenuItem("Import")) {
                        m_ConvertAssetWindowActive = true;
                    }
                }
                ImGui::EndPopup();
            }

            ImGui::TextWrapped(entry.filename().string().c_str());

            item_id++;
        }
        ImGui::EndTable();
        if (filesystem_refetch_requested)
            FetchCurrentDirectory();
    }

    ImGui::End();
}

void ContentBrowser::FetchCurrentDirectory()
{
    m_CurrentDirectoryEntries.clear();
    std::filesystem::directory_iterator it(m_CurrentDirectory);
    for (auto& entry : it) {
        if (entry.is_directory())
            m_CurrentDirectoryEntries.insert(m_CurrentDirectoryEntries.begin(), entry.path());
        else
            m_CurrentDirectoryEntries.push_back(entry.path());
    }
}
}
