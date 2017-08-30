#include "pch.h"
#include "HologramJS.h"
#include "ImageElement.h"
#include "ScriptsLoader.h"
#include "System.h"
#include "VideoElement.h"
#include "WebGLProjections.h"
#include "XmlHttpRequest.h"
#include "ScriptHostUtilities.h"

using namespace HologramJS;
using namespace Platform;
using namespace concurrency;
using namespace Windows::Foundation;
using namespace std;
using namespace Windows::Perception::Spatial;
using namespace Windows::ApplicationModel;

HologramScriptHost::HologramScriptHost() {}

HologramScriptHost::~HologramScriptHost() { Shutdown(); }

void HologramScriptHost::Shutdown()
{
    m_window.Close();

    if (m_jsContext != nullptr) {
        JsSetCurrentContext(nullptr);
        m_jsContext = nullptr;
    }

    if (m_jsRuntime != nullptr) {
        JsDisposeRuntime(m_jsRuntime);
        m_jsRuntime = nullptr;
    }
}

bool HologramScriptHost::Initialize()
{
    RETURN_IF_JS_ERROR(JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &m_jsRuntime));
    RETURN_IF_JS_ERROR(JsCreateContext(m_jsRuntime, &m_jsContext));

    RETURN_IF_JS_ERROR(JsSetCurrentContext(m_jsContext));

    JsStartDebugging();

    RETURN_IF_FALSE(m_scriptEventsManager.Initialize());

    RETURN_IF_FALSE(API::XmlHttpRequest::Initialize());

    RETURN_IF_FALSE(API::ImageElement::Initialize());
    RETURN_IF_FALSE(API::VideoElement::Initialize());

    RETURN_IF_FALSE(WebGL::WebGLProjections::Initialize());

    RETURN_IF_FALSE(m_window.Initialize());

    RETURN_IF_FALSE(m_system.Initialize());

    RETURN_IF_FALSE(m_timers.Initialize());

    return true;
}

IAsyncOperation<bool> ^ HologramScriptHost::RunApp(Platform::String ^ appUri)
{
    wstring appUriString = appUri->Data();

    if (ScriptsLoader::IsAppUriLocal(appUriString))
    {
        return create_async([this, appUri]() -> task<bool> { return RunLocalScriptApp(appUri->Data()); });
    }
    else
    {
        return create_async([this, appUri]() -> task<bool> { return RunWebScriptApp(appUri->Data()); });
    }
}

task<bool> HologramScriptHost::RunLocalScriptApp(wstring jsonFilePath)
{
    unique_ptr<HologramJS::ScriptsLoader> loader(new ScriptsLoader());
    auto loadResult = await loader->LoadScriptsAsync(Package::Current->InstalledLocation,
                                                L"hologramjs\\scriptingframework\\framework.json");

    if (loadResult) {
        loadResult =
            await loader->LoadScriptsAsync(Package::Current->InstalledLocation, jsonFilePath);
    }

    if (loadResult) {
        API::XmlHttpRequest::UseFileSystem = true;
        API::ImageElement::UseFileSystem = true;
        API::VideoElement::UseFileSystem = true;

        const wstring basePath = ScriptsLoader::GetFileSystemBasePathForJsonPath(jsonFilePath);
        API::XmlHttpRequest::BasePath = basePath;
        API::ImageElement::BasePath = basePath;
        API::VideoElement::BasePath = basePath;

        m_window.SetBaseUrl(basePath);

        loader->ExecuteScripts();
    }

    return true;
}

task<bool> HologramScriptHost::RunWebScriptApp(wstring jsonUri)
{
    unique_ptr<HologramJS::ScriptsLoader> loader(new HologramJS::ScriptsLoader());
    wstring jsonUriString = jsonUri;
    auto loadResult = await loader->LoadScriptsAsync(Package::Current->InstalledLocation,
                                                L"hologramjs\\scriptingframework\\framework.json");

    if (loadResult) {
        loadResult = await loader->DownloadScriptsAsync(jsonUriString);
    }

    if (loadResult) {
        API::XmlHttpRequest::UseFileSystem = false;
        API::ImageElement::UseFileSystem = false;
        API::VideoElement::UseFileSystem = false;

        // Build base path without the .json file name it in
        wstring basePath = ScriptsLoader::GetBaseUriForJsonUri(jsonUriString);

        m_window.SetBaseUrl(basePath);

        API::XmlHttpRequest::BaseUrl.assign(basePath);
        API::ImageElement::BaseUrl.assign(basePath);
        API::VideoElement::BaseUrl.assign(basePath);

        loader->ExecuteScripts();
    }

    return true;
}

bool HologramScriptHost::EnableHolographicExperimental(SpatialStationaryFrameOfReference ^ frameOfReference)
{
    // Get the global object
    JsValueRef globalObject;

    RETURN_IF_JS_ERROR(JsGetGlobalObject(&globalObject));

    // Create or get global.nativeInterface
    JsValueRef windowRef;
    RETURN_IF_FALSE(Utilities::ScriptHostUtilities::GetJsProperty(globalObject, L"window", &windowRef));

    JsPropertyIdRef holographicPropertyId;
    RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(L"experimentalHolographic", &holographicPropertyId));
    JsValueRef holographicValue;
    RETURN_IF_JS_ERROR(JsBoolToBoolean(true, &holographicValue));

    RETURN_IF_JS_ERROR(JsSetProperty(windowRef, holographicPropertyId, holographicValue, true));

    m_window.SetStationaryFrameOfReference(frameOfReference);

    return true;
}