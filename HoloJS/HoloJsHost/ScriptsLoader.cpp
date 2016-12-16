#include "pch.h"
#include "ScriptsLoader.h"

using namespace concurrency;
using namespace std;
using namespace Windows::Web;
using namespace Windows::Foundation;

namespace HologramJS
{

ScriptsLoader::ScriptsLoader()
{
}


ScriptsLoader::~ScriptsLoader()
{
}

task<bool>
ScriptsLoader::DownloadScripts(
	const std::wstring& uri
)
{
	if (uri.rfind(L".json") != (uri.length() - wcslen(L".json")))
	{
		return false;
	}

	wstring rawUri = uri;
	if (rawUri.find(L"web-ar") == 0)
	{
		rawUri.replace(0, wcslen(L"web-ar"), L"http");
	}

	auto modifiedUri = ref new Windows::Foundation::Uri(Platform::StringReference(rawUri.c_str()));

	auto scriptsJson = await DownloadTextFromURI(modifiedUri);

	auto scriptsList = GetScriptsListFromJSON(scriptsJson);

	// Build base path without the .json file name it in
	wstring basePath = GetBaseUriForJsonUri(uri);
	
	for (const auto& scriptRelativeUri : scriptsList)
	{
		auto scriptCompleteUri = basePath + L"/" + scriptRelativeUri;
		
		auto scriptUri = ref new Uri(Platform::StringReference(scriptCompleteUri.c_str()));

		auto scriptPlatformText = await DownloadTextFromURI(scriptUri);
		wstring scriptText = scriptPlatformText->Data();
		if (!scriptText.empty())
		{
			m_loadedScripts.emplace_back(std::move(scriptText));
		}
	}

	return true;
}

task<bool>
ScriptsLoader::LoadScripts(
	Windows::Storage::StorageFolder^ root,
	const wstring& jsonPath)
{
	auto pathElements = SplitPath(jsonPath);
	if (pathElements.empty())
	{
		return false;
	}

	auto jsonFileName = pathElements.front();
	pathElements.erase(pathElements.begin());

	if (jsonFileName.rfind(L".json") != (jsonFileName.length() - wcslen(L".json")))
	{
		return false;
	}

	auto scriptsJson = await ReadTextFromFile(root, pathElements, jsonFileName);

	auto scriptsList = GetScriptsListFromJSON(scriptsJson);

	for (const auto& scriptPath : scriptsList)
	{
		auto scriptPathElements = SplitPath(scriptPath);
		if (!scriptPathElements.empty())
		{
			auto scriptName = scriptPathElements.front();
			scriptPathElements.erase(scriptPathElements.begin());

			// Merge JSON path with script path; scripts are relative located from the main JSON
			scriptPathElements.insert(scriptPathElements.begin(), pathElements.begin(), pathElements.end());
			auto scriptPlatformString = await ReadTextFromFile(root, scriptPathElements, scriptName);
			wstring scriptText = scriptPlatformString->Data();
			if (!scriptText.empty())
			{
				m_loadedScripts.emplace_back(std::move(scriptText));
			}
		}
	}

	return true;
}

list<wstring>
ScriptsLoader::GetScriptsListFromJSON(Platform::String^ json)
{
	list<wstring> scriptsList;
	Windows::Data::Json::JsonObject^ jsonObj;
	if (!Windows::Data::Json::JsonObject::TryParse(json, &jsonObj))
	{
		return scriptsList;
	}

	auto scriptsListInJson = jsonObj->GetNamedArray(L"scripts");
	for (unsigned int i = 0; i < scriptsListInJson->Size; i++)
	{
		scriptsList.push_back(scriptsListInJson->GetStringAt(i)->Data());
	}

	return scriptsList;
}

task<Platform::String^>
ScriptsLoader::ReadTextFromFile(
	Windows::Storage::StorageFolder^ rootFolder,
	std::list<std::wstring>& pathElements,
	std::wstring& fileName
)
{
	auto currentFolder = rootFolder;
	for (const auto& folder : pathElements)
	{
		currentFolder = await currentFolder->GetFolderAsync(Platform::StringReference(folder.c_str()));
	}
	
	auto file = await currentFolder->GetFileAsync(Platform::StringReference(fileName.c_str()));
	return await Windows::Storage::FileIO::ReadTextAsync(file);
}

task<Windows::Storage::Streams::IBuffer^>
ScriptsLoader::ReadBinaryFromFile(
	Windows::Storage::StorageFolder^ rootFolder,
	std::list<std::wstring>& pathElements,
	std::wstring& fileName
)
{
	auto currentFolder = rootFolder;
	for (const auto& folder : pathElements)
	{
		currentFolder = await currentFolder->GetFolderAsync(Platform::StringReference(folder.c_str()));
	}

	auto file = await currentFolder->GetFileAsync(Platform::StringReference(fileName.c_str()));
	return await Windows::Storage::FileIO::ReadBufferAsync(file);
}

task<Windows::Storage::Streams::IRandomAccessStreamWithContentType^>
ScriptsLoader::GetStreamFromFile(
	Windows::Storage::StorageFolder^ rootFolder,
	std::list<std::wstring>& pathElements,
	std::wstring& fileName
)
{
	auto currentFolder = rootFolder;
	for (const auto& folder : pathElements)
	{
		currentFolder = await currentFolder->GetFolderAsync(Platform::StringReference(folder.c_str()));
	}

	auto file = await currentFolder->GetFileAsync(Platform::StringReference(fileName.c_str()));
	auto fileStream = await file->OpenReadAsync();
	return fileStream;
}


task<Platform::String^>
ScriptsLoader::DownloadTextFromURI(
	Windows::Foundation::Uri^ uri
)
{
	Http::HttpClient^ httpClient = ref new  Windows::Web::Http::HttpClient();
	auto responseMessage = await httpClient->GetAsync(uri);
	if (responseMessage->IsSuccessStatusCode)
	{
		return await responseMessage->Content->ReadAsStringAsync();
	}

	return L"";
}

list<wstring>
ScriptsLoader::SplitPath(const std::wstring& path)
{
	list<wstring> pathElements;
	if (path.empty())
	{
		return pathElements;
	}

	std::experimental::filesystem::path parsedPath(path);
	auto fileName = parsedPath.filename();
	auto subPath = parsedPath.parent_path();

	while (!subPath.empty())
	{
		auto folderName = subPath.filename();
		if (!folderName.empty() && (folderName.compare("\\") != 0) && (folderName.compare("/") != 0))
		{
			pathElements.push_back(folderName.c_str());
		}

		subPath = subPath.parent_path();
	}

	if (!fileName.empty())
	{
		pathElements.push_back(fileName.c_str());
	}

	pathElements.reverse();

	return pathElements;
}

wstring
ScriptsLoader::GetFileSystemBasePathForJsonPath(
	const wstring& jsonFilePath
)
{
	auto mainPathElements = SplitPath(jsonFilePath);
	if (mainPathElements.size() == 0)
	{
		return L"\\";
	}

	auto jsonFileName = mainPathElements.front();;
	mainPathElements.erase(mainPathElements.begin());;

	wstring returnPath = L"";
	for (const auto& pathElement : mainPathElements)
	{
		returnPath += pathElement + L"\\";
	}

	return returnPath;
}

wstring
ScriptsLoader::GetBaseUriForJsonUri(
	const wstring& jsonUri
)
{
	wstring localUri = jsonUri;
	auto lastSegmentIndex = localUri.rfind('/');
	if (lastSegmentIndex == wstring::npos || lastSegmentIndex == 0)
	{
		return localUri;
	}

	if (localUri.find(L"web-ar") == 0)
	{
		localUri.replace(0, wcslen(L"web-ar"), L"http");
	}

	// Build base path without the .json file name it in
	return localUri.substr(0, lastSegmentIndex - 1);
}

}