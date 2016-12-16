#pragma once

namespace HologramJS
{
	class ScriptsLoader
	{
	public:
		ScriptsLoader();
		~ScriptsLoader();

		concurrency::task<bool> LoadScripts(Windows::Storage::StorageFolder^ root, const std::wstring& jsonPath);
		concurrency::task<bool> DownloadScripts(const std::wstring& uri);

		const std::list<std::wstring>& LoadedScripts() { return m_loadedScripts; }

		static std::list<std::wstring> SplitPath(const std::wstring& path);

		std::list<std::wstring> GetScriptsListFromJSON(Platform::String^ json);

		static concurrency::task<Platform::String^> ReadTextFromFile(
			Windows::Storage::StorageFolder^ rootFolder,
			std::list<std::wstring>& pathElements,
			std::wstring& fileName);

		static concurrency::task<Platform::String^> DownloadTextFromURI(Windows::Foundation::Uri^ uri);

		static concurrency::task<Windows::Storage::Streams::IBuffer^> ReadBinaryFromFile(
			Windows::Storage::StorageFolder^ rootFolder,
			std::list<std::wstring>& pathElements,
			std::wstring& fileName);

		static concurrency::task<Windows::Storage::Streams::IRandomAccessStreamWithContentType^> GetStreamFromFile(
			Windows::Storage::StorageFolder^ rootFolder,
			std::list<std::wstring>& pathElements,
			std::wstring& fileName);

		static std::wstring GetFileSystemBasePathForJsonPath(const std::wstring& jsonFilePath);
		static std::wstring GetBaseUriForJsonUri(const std::wstring& jsonUri);

		std::list<std::wstring> m_loadedScripts;
	};
}
