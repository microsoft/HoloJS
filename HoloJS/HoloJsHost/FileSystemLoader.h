#pragma once

namespace HologramJS {
namespace Loaders {
class FileSystemLoader {
   public:
    FileSystemLoader();
    ~FileSystemLoader();

    static concurrency::task<Windows::Storage::Streams::IBuffer ^> ReadBinaryFromFileAsync(
        Windows::Storage::StorageFolder ^ rootFolder, const std::wstring& path);

    static concurrency::task<Platform::String ^> ReadTextFromFileAsync(Windows::Storage::StorageFolder ^ rootFolder,
                                                                       const std::wstring& path);

    static concurrency::task<Windows::Storage::Streams::IRandomAccessStreamWithContentType ^> GetStreamFromFileAsync(
        Windows::Storage::StorageFolder ^ rootFolder, const std::wstring& path);

   protected:
    static concurrency::task<Windows::Storage::Streams::IBuffer ^> ReadBinaryFromFileAsync(
        Windows::Storage::StorageFolder ^ rootFolder,
        const std::list<std::wstring>& pathElements,
        const std::wstring& fileName);

    static concurrency::task<Platform::String ^> ReadTextFromFileAsync(Windows::Storage::StorageFolder ^ rootFolder,
                                                                       std::list<std::wstring>& pathElements,
                                                                       std::wstring& fileName);

    static concurrency::task<Windows::Storage::Streams::IRandomAccessStreamWithContentType ^> GetStreamFromFileAsync(
        Windows::Storage::StorageFolder ^ rootFolder,
        const std::list<std::wstring>& pathElements,
        const std::wstring& fileName);

    static std::list<std::wstring> SplitPath(const std::wstring& path);
};

}  // namespace Loaders
}  // namespace HologramJS