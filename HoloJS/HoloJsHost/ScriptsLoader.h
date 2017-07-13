#pragma once

#include "ChakraForHoloJS.h"

namespace HologramJS {
struct Script {
    std::wstring path;
    std::wstring code;
};

class ScriptsLoader {
   public:
    ScriptsLoader();
    ~ScriptsLoader();

    concurrency::task<bool> LoadScripts(Windows::Storage::StorageFolder ^ root, const std::wstring& jsonPath);
    concurrency::task<bool> DownloadScripts(const std::wstring& uri);

    static std::list<std::wstring> SplitPath(const std::wstring& path);

    std::list<std::wstring> GetScriptsListFromJSON(Platform::String ^ json);

    static concurrency::task<Platform::String ^> ReadTextFromFile(Windows::Storage::StorageFolder ^ rootFolder,
                                                                  std::list<std::wstring>& pathElements,
                                                                  std::wstring& fileName);

    static concurrency::task<Platform::String ^> DownloadTextFromURI(Windows::Foundation::Uri ^ uri);

    static concurrency::task<Windows::Storage::Streams::IBuffer ^> ReadBinaryFromFile(
        Windows::Storage::StorageFolder ^ rootFolder, std::list<std::wstring>& pathElements, std::wstring& fileName);

    static concurrency::task<Windows::Storage::Streams::IRandomAccessStreamWithContentType ^> GetStreamFromFile(
        Windows::Storage::StorageFolder ^ rootFolder, std::list<std::wstring>& pathElements, std::wstring& fileName);

    static std::wstring GetFileSystemBasePathForJsonPath(const std::wstring& jsonFilePath);
    static std::wstring GetBaseUriForJsonUri(const std::wstring& jsonUri);

    void ExecuteScripts();

    std::list<std::shared_ptr<Script>> m_loadedScripts;
    JsSourceContext m_jsSourceContext = 0;
};
}  // namespace HologramJS
