#pragma once

#include <Tabby.h>
#include <Tabby/Core/Time/Timer.h>

namespace Editor {

class ContentBrowser {
public:
    ContentBrowser();
    ~ContentBrowser();

    void OnImGuiRender();

private:
    void FetchCurrentDirectory();

private:
    std::filesystem::path m_WorkingDirectory;
    std::filesystem::path m_CurrentDirectory;

    std::unordered_map<std::string, Tabby::Shared<Tabby::Texture>> m_IconMap;
    Tabby::Timer m_DirectoryFetchTimer;
    std::vector<std::filesystem::path> m_CurrentDirectoryEntries;

    bool m_CreateDirectoryWindowActive = false;
    bool m_ConvertAssetWindowActive = false;
};

}
