#pragma once
#include "script-loader.h"
#include "include/holojs/private/app-model.h"
#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/platform-interfaces.h"
#include <filesystem>
#include <string>

using namespace HoloJs::AppModel;
using namespace std;

long ScriptsLoader::loadApp(const std::wstring& appUri, shared_ptr<HoloJsApp>& scriptApp)
{
    static wstring packageExtension = L".xrsx";
    AppConfiguration appConfig;

    if (appUri.rfind(packageExtension) == appUri.length() - packageExtension.length())
    {
        appConfig.source = AppSource::Package;
    } else {
        appConfig.source = isAbsoluteWebUri(appUri) ? AppSource::Web : AppSource::FileSystem;
    }

    wstring appJson;
    if (appConfig.source == AppSource::Web) {
        RETURN_IF_FAILED(getPlatform()->downloadText(appUri, appJson));
        appConfig.baseUri = getBaseUriForAppUri(appUri);
    } else if (appConfig.source == AppSource::FileSystem) {
        RETURN_IF_FAILED(getPlatform()->readFileUTF8(appUri, appJson));
        appConfig.baseUri = getFileSystemBasePathForAppPath(appUri);
    } else if (appConfig.source == AppSource::Package) {
        appConfig.packageReader.reset(getPlatform()->getPackageReaderFromPath(appUri));
		RETURN_IF_NULL(appConfig.packageReader.get());
        RETURN_IF_FAILED(appConfig.packageReader->readFileFromPackageUTF8(L"app.json", appJson));
        appConfig.baseUri = L"\\";
    }

    shared_ptr<HoloJsApp> newScriptApp = make_shared<HoloJsApp>(appConfig);

    list<wstring> scriptsList;
    wstring appName = L"";
    wstring appIconUri = L"";
    RETURN_IF_FAILED(getPlatform()->getAppConfigFromJson(appJson, scriptsList, appName, appIconUri));

    newScriptApp->setName(appName);

    if (appConfig.source == AppSource::Web) {
        auto iconFinalUri = appConfig.baseUri + L"/" + appIconUri;
        newScriptApp->setIconUri(iconFinalUri);
    } else if (appConfig.source == AppSource::FileSystem) {
        experimental::filesystem::path iconFinalPath(appConfig.baseUri);
        iconFinalPath.append(appIconUri);
        wstring iconFinalPathString = iconFinalPath;
        newScriptApp->setIconUri(iconFinalPathString);
    } else if (appConfig.source == AppSource::Package) {
        newScriptApp->setIconUri(appIconUri);
    }

    // First add the support scripts
    for (const auto& supportScript : *m_supportScripts)
    {
        auto scriptCopy = supportScript.getCode();
        newScriptApp->loadedScriptsList()->emplace_back(supportScript.getPath(), move(scriptCopy));
        newScriptApp->loadedScriptsList()->back().setName(supportScript.getName());
    }

    for (const auto& scriptName : scriptsList) {
        wstring scriptUri;
        wstring scriptText;
        if (appConfig.source == AppSource::Web) {
            scriptUri = appConfig.baseUri + L"/" + scriptName;
            try {
                RETURN_IF_FAILED(getPlatform()->downloadText(scriptUri, scriptText));
            } catch (...) {
                return HoloJs::Error;
            }
        } else if (appConfig.source == AppSource::FileSystem) {
            experimental::filesystem::path scriptPath(appConfig.baseUri);
            scriptPath.append(scriptName);
            scriptUri = scriptPath.c_str();
            RETURN_IF_FAILED(getPlatform()->readFileUTF8(scriptUri, scriptText));
        } else if (appConfig.source == AppSource::Package) {
            scriptUri = scriptName;
            RETURN_IF_FAILED(appConfig.packageReader->readFileFromPackageUTF8(scriptUri, scriptText));
        }

        newScriptApp->loadedScriptsList()->emplace_back(scriptUri, move(scriptText));
		newScriptApp->loadedScriptsList()->back().setName(scriptName);
    }

    scriptApp = newScriptApp;

    return HoloJs::Success;
}

long ScriptsLoader::loadApp(void* platformHandle, std::shared_ptr<HoloJsApp>& scriptApp)
{
    AppConfiguration appConfig;
    appConfig.source = AppSource::Package;
    appConfig.baseUri = L"\\";

    appConfig.packageReader.reset(getPlatform()->getPackageReaderFromHandle(platformHandle));

    wstring appJson;
    RETURN_IF_FAILED(appConfig.packageReader->readFileFromPackageUTF8(L"app.xrs", appJson));
   
    shared_ptr<HoloJsApp> newScriptApp = make_shared<HoloJsApp>(appConfig);

    list<wstring> scriptsList;
    wstring appName = L"";
    wstring appIconUri = L"";
    RETURN_IF_FAILED(getPlatform()->getAppConfigFromJson(appJson, scriptsList, appName, appIconUri));

    newScriptApp->setName(appName);
    newScriptApp->setIconUri(appIconUri);

    // First add the support scripts
    for (const auto& supportScript : *m_supportScripts) {
        auto scriptCopy = supportScript.getCode();
        newScriptApp->loadedScriptsList()->emplace_back(supportScript.getPath(), move(scriptCopy));
        newScriptApp->loadedScriptsList()->back().setName(supportScript.getName());
    }

    for (const auto& scriptName : scriptsList) {
        wstring scriptUri;
        wstring scriptText;
        
        scriptUri = scriptName;
        RETURN_IF_FAILED(appConfig.packageReader->readFileFromPackageUTF8(scriptUri, scriptText));

        newScriptApp->loadedScriptsList()->emplace_back(scriptUri, move(scriptText));
        newScriptApp->loadedScriptsList()->back().setName(scriptName);
    }

    scriptApp = newScriptApp;

    return HoloJs::Success;
}

long ScriptsLoader::createEmptyApp(std::shared_ptr<HoloJsApp>& scriptApp)
{
    AppConfiguration appConfig;
    appConfig.source = AppSource::FileSystem;
    appConfig.baseUri = L"\\";

    shared_ptr<HoloJsApp> newScriptApp = make_shared<HoloJsApp>(appConfig);

    list<wstring> scriptsList;
    wstring appName = L"";
    wstring appIconUri = L"";

    newScriptApp->setName(appName);
    newScriptApp->setIconUri(appIconUri);

    // Add just the support scripts
    for (const auto& supportScript : *m_supportScripts) {
        auto scriptCopy = supportScript.getCode();
        newScriptApp->loadedScriptsList()->emplace_back(supportScript.getPath(), move(scriptCopy));
        newScriptApp->loadedScriptsList()->back().setName(supportScript.getName());
    }

    scriptApp = newScriptApp;

    return HoloJs::Success;
}

long ScriptsLoader::createAppFromScripts(const wstring& appName,
                                         shared_ptr<list<HoloJs::AppModel::Script>> scripts,
                                         std::shared_ptr<HoloJsApp>& scriptApp)
{
    AppConfiguration appConfig;
    appConfig.source = AppSource::FileSystem;
    appConfig.baseUri = L"\\";

    shared_ptr<HoloJsApp> newScriptApp = make_shared<HoloJsApp>(appConfig);

    list<wstring> scriptsList;
    wstring appIconUri = L"";

    newScriptApp->setName(appName);
    newScriptApp->setIconUri(appIconUri);

    // First add the support scripts
    for (const auto& supportScript : *m_supportScripts) {
        auto scriptCopy = supportScript.getCode();
        newScriptApp->loadedScriptsList()->emplace_back(supportScript.getPath(), move(scriptCopy));
        newScriptApp->loadedScriptsList()->back().setName(supportScript.getName());
    }

    for (const auto& appScript : *scripts.get()) {
        auto scriptCopy = appScript.getCode();
        newScriptApp->loadedScriptsList()->emplace_back(appScript.getPath(), move(scriptCopy));
        newScriptApp->loadedScriptsList()->back().setName(appScript.getName());
    }

    scriptApp = newScriptApp;

    return HoloJs::Success;
}

long ScriptsLoader::loadScriptInline(const std::wstring& script, shared_ptr<HoloJsApp>& scriptApp)
{
    AppConfiguration appConfig;
    appConfig.source = AppSource::FileSystem;
    
    RETURN_IF_FAILED(getPlatform()->getCurrentDirectory(appConfig.baseUri));

    auto newScriptApp = make_shared<HoloJsApp>(appConfig);

    // First add the support scripts
    for (const auto& supportScript : *m_supportScripts) {
        auto scriptCopy = supportScript.getCode();
        newScriptApp->loadedScriptsList()->emplace_back(supportScript.getPath(), move(scriptCopy));
        newScriptApp->loadedScriptsList()->back().setName(supportScript.getName());
    }

    // Make a copy of the script code
    auto scriptCode = script;
    newScriptApp->loadedScriptsList()->emplace_back(L"inline", move(scriptCode));

    scriptApp = newScriptApp;

    return HoloJs::Success;
}

bool ScriptsLoader::isAbsoluteWebUri(std::wstring uri)
{
    return (_wcsnicmp(uri.c_str(), L"http://", wcslen(L"http://")) == 0) ||
           (_wcsnicmp(uri.c_str(), L"https://", wcslen(L"https://")) == 0);
}

wstring ScriptsLoader::getFileSystemBasePathForAppPath(const std::wstring& appPath)
{
    auto mainPathElements = splitPath(appPath);
    if (mainPathElements.size() == 0) {
        return L"\\";
    }

    auto jsonFileName = mainPathElements.front();

    mainPathElements.erase(mainPathElements.begin());

    wstring returnPath = L"";
    for (const auto& pathElement : mainPathElements) {
        returnPath += pathElement + L"\\";
    }

    return returnPath;
}

wstring ScriptsLoader::getBaseUriForAppUri(const wstring& appUri)
{
    auto lastSegmentIndex = appUri.rfind('/');
    if (lastSegmentIndex == wstring::npos || lastSegmentIndex == 0) {
        return appUri;
    }

    // Build base path without the json file name it in
    return appUri.substr(0, lastSegmentIndex);
}

list<wstring> ScriptsLoader::splitPath(const std::wstring& path)
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
