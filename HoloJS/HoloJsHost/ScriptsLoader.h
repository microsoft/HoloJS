#pragma once

#include "ChakraForHoloJS.h"
#include "FileSystemLoader.h"
#include "WebLoader.h"

namespace HologramJS {
struct Script {
    std::wstring path;
    std::wstring code;
};

class ScriptsLoader : HologramJS::Loaders::FileSystemLoader, HologramJS::Loaders::WebLoader {
   public:
    ScriptsLoader() {}

    concurrency::task<bool> LoadScriptsAsync(Windows::Storage::StorageFolder ^ root, const std::wstring& jsonPath);
    concurrency::task<bool> DownloadScriptsAsync(const std::wstring& uri);

    std::list<std::wstring> GetScriptsListFromJSON(Platform::String ^ json);

    static std::wstring GetFileSystemBasePathForJsonPath(const std::wstring& jsonFilePath);
    static std::wstring GetBaseUriForJsonUri(const std::wstring& jsonUri);

    static bool IsAbsoluteWebUri(std::wstring appUri);

    void ExecuteScripts();

   private:

    std::list<std::shared_ptr<Script>> m_loadedScripts;
    JsSourceContext m_jsSourceContext = 0;
};
}  // namespace HologramJS
