#include "pch.h"
#include "FileSystemLoader.h"

using namespace concurrency;
using namespace std;
using namespace Windows::Web;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Data::Json;
using namespace Windows::Storage::Streams;
using namespace Windows::Web::Http;
using namespace HologramJS::Loaders;

FileSystemLoader::FileSystemLoader() {}

FileSystemLoader::~FileSystemLoader() {}

concurrency::task<IBuffer ^> FileSystemLoader::ReadBinaryFromFileAsync(StorageFolder ^ rootFolder, const wstring& path)
{
    auto mainPathElements = SplitPath(path);

    if (mainPathElements.size() == 0) {
        return nullptr;
    }

    auto fileName = mainPathElements.front();
    mainPathElements.erase(mainPathElements.begin());

    return await ReadBinaryFromFileAsync(rootFolder, mainPathElements, fileName);
}

concurrency::task<Platform::String ^> FileSystemLoader::ReadTextFromFileAsync(StorageFolder ^ rootFolder,
                                                                              const wstring& path)
{
    auto mainPathElements = SplitPath(path);

    if (mainPathElements.size() == 0) {
        return nullptr;
    }

    auto fileName = mainPathElements.front();
    mainPathElements.erase(mainPathElements.begin());

    return await ReadTextFromFileAsync(rootFolder, mainPathElements, fileName);
}

task<Platform::String ^> FileSystemLoader::ReadTextFromFileAsync(StorageFolder ^ rootFolder,
                                                                 list<std::wstring>& pathElements,
                                                                 wstring& fileName)
{
    auto currentFolder = rootFolder;
    for (const auto& folder : pathElements) {
        currentFolder = await currentFolder->GetFolderAsync(Platform::StringReference(folder.c_str()));
    }

    auto file = await currentFolder->GetFileAsync(Platform::StringReference(fileName.c_str()));
    return await FileIO::ReadTextAsync(file);
}

task<IBuffer ^> FileSystemLoader::ReadBinaryFromFileAsync(StorageFolder ^ rootFolder,
                                                          const list<wstring>& pathElements,
                                                          const wstring& fileName)
{
    auto currentFolder = rootFolder;
    for (const auto& folder : pathElements) {
        currentFolder = await currentFolder->GetFolderAsync(Platform::StringReference(folder.c_str()));
    }

    auto file = await currentFolder->GetFileAsync(Platform::StringReference(fileName.c_str()));
    return await FileIO::ReadBufferAsync(file);
}

task<IRandomAccessStreamWithContentType ^> FileSystemLoader::GetStreamFromFileAsync(StorageFolder ^ rootFolder,
                                                                                    const list<wstring>& pathElements,
                                                                                    const wstring& fileName)
{
    auto currentFolder = rootFolder;
    for (const auto& folder : pathElements) {
        currentFolder = await currentFolder->GetFolderAsync(Platform::StringReference(folder.c_str()));
    }

    auto file = await currentFolder->GetFileAsync(Platform::StringReference(fileName.c_str()));
    auto fileStream = await file->OpenReadAsync();
    return fileStream;
}

list<wstring> FileSystemLoader::SplitPath(const std::wstring& path)
{
    list<wstring> pathElements;
    if (path.empty()) {
        return pathElements;
    }

    std::experimental::filesystem::path parsedPath(path);
    auto fileName = parsedPath.filename();
    auto subPath = parsedPath.parent_path();

    while (!subPath.empty()) {
        auto folderName = subPath.filename();
        if (!folderName.empty() && (folderName.compare("\\") != 0) && (folderName.compare("/") != 0)) {
            pathElements.push_back(folderName.c_str());
        }

        subPath = subPath.parent_path();
    }

    if (!fileName.empty()) {
        pathElements.push_back(fileName.c_str());
    }

    pathElements.reverse();

    return pathElements;
}
