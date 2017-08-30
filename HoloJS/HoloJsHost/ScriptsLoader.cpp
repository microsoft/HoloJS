#include "pch.h"
#include "ScriptsLoader.h"
#include "ScriptErrorHandling.h"

using namespace concurrency;
using namespace std;
using namespace Windows::Web;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Data::Json;
using namespace Windows::Web::Http;
using namespace HologramJS::Loaders;

namespace HologramJS {

task<bool> ScriptsLoader::LoadScriptsAsync(StorageFolder ^ root, const wstring& jsonPath)
{
    auto pathElements = SplitPath(jsonPath);
    if (pathElements.empty()) {
        return false;
    }

    auto jsonFileName = pathElements.front();
    pathElements.erase(pathElements.begin());

    if (jsonFileName.rfind(L".json") != (jsonFileName.length() - wcslen(L".json"))) {
        return false;
    }

    auto scriptsJson = await ReadTextFromFileAsync(root, pathElements, jsonFileName);

    auto scriptsList = GetScriptsListFromJSON(scriptsJson);

    for (const auto& scriptPath : scriptsList) {
        auto scriptPathElements = SplitPath(scriptPath);

        // Allow mixing Web scripts with local scripts
        // When referencing web scripts in a local app, only absolute URIs are allowed
        if (IsAbsoluteWebUri(scriptPath)) {
            auto scriptUri = ref new Uri(Platform::StringReference(scriptPath.c_str()));
            auto scriptPlatformText = await DownloadTextAsync(scriptUri);
            wstring scriptText = scriptPlatformText->Data();
            if (!scriptText.empty()) {
                std::shared_ptr<Script> script(new Script());
                script->path = move(scriptUri->AbsoluteUri->Data());
                script->code = move(scriptText);
                m_loadedScripts.emplace_back(script);
            }
        } else if (!scriptPathElements.empty()) {
            auto scriptName = scriptPathElements.front();
            scriptPathElements.erase(scriptPathElements.begin());

            // Merge JSON path with script path; scripts are relative located from the main JSON
            scriptPathElements.insert(scriptPathElements.begin(), pathElements.begin(), pathElements.end());
            auto scriptPlatformString = await ReadTextFromFileAsync(root, scriptPathElements, scriptName);
            wstring scriptText = scriptPlatformString->Data();
            if (!scriptText.empty()) {
                std::shared_ptr<Script> script(new Script());
                script->path = move(scriptName);
                script->code = move(scriptText);
                m_loadedScripts.emplace_back(script);
            }
        }
    }

    return true;
}

task<bool> ScriptsLoader::DownloadScriptsAsync(const std::wstring& uri)
{
    if (uri.rfind(L".json") != (uri.length() - wcslen(L".json"))) {
        return false;
    }

    auto modifiedUri = ref new Uri(Platform::StringReference(uri.c_str()));

    auto scriptsJson = await DownloadTextAsync(modifiedUri);

    auto scriptsList = GetScriptsListFromJSON(scriptsJson);

    // Build base path without the .json file name it in
    wstring basePath = GetBaseUriForJsonUri(uri);

    for (const auto& scriptRelativeUri : scriptsList) {
        auto scriptCompleteUri = basePath + L"/" + scriptRelativeUri;

        auto scriptUri = ref new Uri(Platform::StringReference(scriptCompleteUri.c_str()));

        auto scriptPlatformText = await DownloadTextAsync(scriptUri);
        wstring scriptText = scriptPlatformText->Data();
        if (!scriptText.empty()) {
            std::shared_ptr<Script> script(new Script());
            script->path = move(scriptUri->AbsoluteUri->Data());
            script->code = move(scriptText);
            m_loadedScripts.emplace_back(script);
        }
    }

    return true;
}


list<wstring> ScriptsLoader::GetScriptsListFromJSON(Platform::String ^ json)
{
    list<wstring> scriptsList;
    JsonObject ^ jsonObj;
    if (!JsonObject::TryParse(json, &jsonObj)) {
        return scriptsList;
    }

    auto scriptsListInJson = jsonObj->GetNamedArray(L"scripts");
    for (unsigned int i = 0; i < scriptsListInJson->Size; i++) {
        scriptsList.push_back(scriptsListInJson->GetStringAt(i)->Data());
    }

    return scriptsList;
}

wstring ScriptsLoader::GetFileSystemBasePathForJsonPath(const wstring& jsonFilePath)
{
    auto mainPathElements = SplitPath(jsonFilePath);
    if (mainPathElements.size() == 0) {
        return L"\\";
    }

    auto jsonFileName = mainPathElements.front();
    ;
    mainPathElements.erase(mainPathElements.begin());
    ;

    wstring returnPath = L"";
    for (const auto& pathElement : mainPathElements) {
        returnPath += pathElement + L"\\";
    }

    return returnPath;
}

wstring ScriptsLoader::GetBaseUriForJsonUri(const wstring& jsonUri)
{
    wstring localUri = jsonUri;

    auto lastSegmentIndex = localUri.rfind('/');
    if (lastSegmentIndex == wstring::npos || lastSegmentIndex == 0) {
        return localUri;
    }

    // Build base path without the json file name it in
    return localUri.substr(0, lastSegmentIndex);
}

void ScriptsLoader::ExecuteScripts()
{
    for (const auto& script : m_loadedScripts) {
        JsValueRef result;
        HANDLE_EXCEPTION_IF_JS_ERROR(
            JsRunScript(script->code.c_str(), m_jsSourceContext++, script->path.c_str(), &result));
    }
}

bool ScriptsLoader::IsAbsoluteWebUri(std::wstring appUri)
{
    return (_wcsnicmp(appUri.c_str(), L"http://", wcslen(L"http://")) == 0) ||
        (_wcsnicmp(appUri.c_str(), L"https://", wcslen(L"https://")) == 0);
}

}  // namespace HologramJS