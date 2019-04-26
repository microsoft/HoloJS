#include "window-element.h"
#include "host-interfaces.h"
#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/holojs-script-host-internal.h"
#include "include/holojs/private/platform-interfaces.h"
#include "include/holojs/private/script-host-utilities.h"
#include <map>
#include <string>

using namespace HoloJs::Interfaces;
using namespace std;

static const std::wstring WidthId = L"width";
static const std::wstring HeightId = L"height";
static const std::wstring SrcElementId = L"srcElement";
static const std::wstring EventTypeId = L"type";

static map<HoloJs::MouseButtonEventType, PCWSTR> MouseToEventNameMap = {
    {HoloJs::MouseButtonEventType::Click, L"click"},
    {HoloJs::MouseButtonEventType::DblClick, L"dblclick"},
    {HoloJs::MouseButtonEventType::Down, L"mousedown"},
    {HoloJs::MouseButtonEventType::Move, L"mousemove"},
    {HoloJs::MouseButtonEventType::Up, L"mouseup"},
    {HoloJs::MouseButtonEventType::Wheel, L"wheel"},
};

void WindowElement::cleanup()
{
    if (m_windowRef != JS_INVALID_REFERENCE) {
        JsRelease(m_windowRef, nullptr);
    }

    if (m_vsyncCallback != JS_INVALID_REFERENCE) {
        JsRelease(m_vsyncCallback, nullptr);
    }

    if (m_noopFunction != JS_INVALID_REFERENCE) {
        JsRelease(m_noopFunction, nullptr);
    }
}

HRESULT WindowElement::initialize()
{
    RETURN_IF_JS_ERROR(JsCreateFunction(noopStatic, nullptr, &m_noopFunction));
    RETURN_IF_JS_ERROR(JsAddRef(m_noopFunction, nullptr));

    JS_PROJECTION_REGISTER(L"window", L"register", register);
    JS_PROJECTION_REGISTER(L"window", L"atob", atob);
    JS_PROJECTION_REGISTER(L"window", L"btoa", btoa);
    JS_PROJECTION_REGISTER(L"window", L"requestPresent", requestPresent);

    RETURN_IF_FAILED(
        m_host->createScriptObject(this, ResourceManagement::ObjectType::Window, &m_windowExternalObjectRef));

    JsValueRef nativeInterfaceRef;
    RETURN_IF_FAILED(ScriptHostUtilities::GetNativeInterfaceJsProperty(&nativeInterfaceRef));

    RETURN_IF_FAILED(ScriptHostUtilities::GetOrCreateJsProperty(nativeInterfaceRef, L"window", &m_nativeInterfaceRef));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_nativeInterfaceRef, L"native", m_windowExternalObjectRef));

    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_nativeInterfaceRef, WidthId, m_width));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_nativeInterfaceRef, HeightId, m_height));


    for (const auto& entry : MouseToEventNameMap) {
        JsValueRef ref;
        RETURN_IF_FAILED(JsPointerToString(entry.second, wcslen(entry.second), &ref));
        RETURN_IF_FAILED(JsAddRef(ref, nullptr));
        m_mouseToEventNameRefMap.emplace(entry.first, ref);
    }

    return HoloJs::Success;
}

JsValueRef WindowElement::_register(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_TRUE(m_isRegistered);
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    m_windowRef = arguments[1];
    m_vsyncCallback = arguments[2];
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(m_windowRef, nullptr));
    RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(m_vsyncCallback, nullptr));

    m_isRegistered = true;

    return JS_INVALID_REFERENCE;
}

JsValueRef WindowElement::_requestPresent(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    JsValueRef isPresentingTrue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsGetTrueValue(&isPresentingTrue));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(arguments[1], L"isPresenting", isPresentingTrue));

    if (m_headsetPresent) {
        m_presentingVrDisplayRef = arguments[1];
        RETURN_INVALID_REF_IF_JS_ERROR(JsAddRef(m_presentingVrDisplayRef, nullptr));
        RETURN_INVALID_REF_IF_FAILED(invokeEventListeners(L"vrdisplaypresentchange"));

        return isPresentingTrue;
    } else {
        JsValueRef isPresentingFalse;
        RETURN_INVALID_REF_IF_JS_ERROR(JsGetFalseValue(&isPresentingFalse));
        return isPresentingFalse;
    }
}

JsValueRef WindowElement::_atob(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    const wchar_t* inputString;
    size_t inputStringLength;
    RETURN_INVALID_REF_IF_JS_ERROR(JsStringToPointer(arguments[1], &inputString, &inputStringLength));

    wstring decodedString;
    getPlatform()->base64Encode(inputString, inputStringLength, decodedString);

    JsValueRef returnValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsPointerToString(decodedString.c_str(), decodedString.length(), &returnValue));

    return returnValue;
}

JsValueRef WindowElement::_btoa( JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    const wchar_t* inputString;
    size_t inputStringLength;
    RETURN_INVALID_REF_IF_JS_ERROR(JsStringToPointer(arguments[1], &inputString, &inputStringLength));

    // TODO: Implement encoding
    wstring encodedString;
    getPlatform()->base64Encode(inputString, inputStringLength, encodedString);

    // Create the string result
    JsValueRef returnValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsPointerToString(encodedString.c_str(), encodedString.length(), &returnValue));

    return returnValue;
}

HRESULT WindowElement::setHeadsetAvailable(bool available)
{
    m_headsetPresent = available;

    JsValueRef nativeInterface;
    RETURN_IF_FAILED(ScriptHostUtilities::GetNativeInterfaceJsProperty(&nativeInterface));

    JsValueRef headsetPresent;
    RETURN_IF_JS_ERROR(JsBoolToBoolean(available, &headsetPresent));
    RETURN_IF_JS_ERROR(ScriptHostUtilities::SetJsProperty(nativeInterface, L"headsetPresent", headsetPresent));

    if (!available && m_presentingVrDisplayRef != JS_INVALID_REFERENCE) {
        JsValueRef isPresentingFalse;
        RETURN_IF_JS_ERROR(JsGetFalseValue(&isPresentingFalse));
        RETURN_IF_FAILED(
            ScriptHostUtilities::SetJsProperty(m_presentingVrDisplayRef, L"isPresenting", isPresentingFalse));

        RETURN_IF_JS_ERROR(JsRelease(m_presentingVrDisplayRef, nullptr));
        m_presentingVrDisplayRef = JS_INVALID_REFERENCE;
    }

    if (available) {
        RETURN_IF_FAILED(invokeEventListeners(L"onvrdisplayconnect"));
    } else {
        RETURN_IF_FAILED(invokeEventListeners(L"onvrdisplaydisconnect"));
    }

    return HoloJs::Success;
}

HRESULT WindowElement::vSync()
{
    if (m_vsyncCallback != JS_INVALID_REFERENCE) {
        JsValueRef result;
        RETURN_IF_JS_ERROR(JsCallFunction(m_vsyncCallback, &m_windowRef, 1, &result));
    }
    

    return HoloJs::Success;
}

HRESULT WindowElement::resize(int width, int height)
{
    const bool shouldFireResize = (m_width != width || m_height != height);

    if (shouldFireResize) {
        m_width = width;
        m_height = height;

        RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_nativeInterfaceRef, WidthId, m_width));
        RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(m_nativeInterfaceRef, HeightId, m_height));

        RETURN_IF_FAILED(invokeEventListeners(L"resize"));
    }

    return HoloJs::Success;
}

long WindowElement::keyEvent(
    HoloJs::KeyEventType type, const std::wstring& key, const std::wstring& code, int location, int keyCode)
{
    static const std::wstring keyId = L"key";
    static const std::wstring codeId = L"code";
    static const std::wstring keyCodeId = L"keyCode";
    static const std::wstring preventDefaultId = L"preventDefault";

    if (m_windowRef == JS_INVALID_REFERENCE) {
        // There is no window element listening for events; skip
        return HoloJs::Success;
    }

    JsValueRef keyboardEvent;
    RETURN_IF_JS_ERROR(JsCreateObject(&keyboardEvent));
    RETURN_IF_JS_ERROR(JsAddRef(keyboardEvent, nullptr));

    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(keyboardEvent, keyId, key));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(keyboardEvent, codeId, code));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(keyboardEvent, keyCodeId, keyCode));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(keyboardEvent, preventDefaultId, m_noopFunction));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(keyboardEvent, SrcElementId, m_windowRef));

    if (type == HoloJs::KeyEventType::KeyUp) {
        RETURN_IF_FAILED(invokeEventListeners(L"keyup", keyboardEvent));
    } else if (type == HoloJs::KeyEventType::KeyDown) {
        RETURN_IF_FAILED(invokeEventListeners(L"keydown", keyboardEvent));
    } else {
        return E_NOTIMPL;
    }

    RETURN_IF_JS_ERROR(JsRelease(keyboardEvent, nullptr));

    return HoloJs::Success;
}

long WindowElement::mouseEvent(HoloJs::MouseButtonEventType type, int x, int y, int button, int buttons)
{
    static const std::wstring xId = L"x";
    static const std::wstring yId = L"y";
    static const std::wstring clientXId = L"clientX";
    static const std::wstring clientYId = L"clientY";
    static const std::wstring buttonId = L"button";
    static const std::wstring buttonsId = L"buttons";
    static const std::wstring deltaYId = L"deltaY";
    static const std::wstring preventDefaultId = L"preventDefault";
    static const std::wstring stopPropagationId = L"stopPropagation";

    if (m_windowRef == JS_INVALID_REFERENCE) {
        // There is no window element listening for events; skip
        return HoloJs::Success;
    }

    auto matchingEventName = MouseToEventNameMap.find(type);
    if (matchingEventName == MouseToEventNameMap.end()) {
        return E_NOTIMPL;
    }

    JsValueRef mouseEvent;
    RETURN_IF_JS_ERROR(JsCreateObject(&mouseEvent));

    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(mouseEvent, xId, x));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(mouseEvent, yId, y));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(mouseEvent, clientXId, x));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(mouseEvent, clientYId, y));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(mouseEvent, buttonId, button));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(mouseEvent, buttonsId, buttons));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(mouseEvent, preventDefaultId, m_noopFunction));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(mouseEvent, stopPropagationId, m_noopFunction));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(mouseEvent, SrcElementId, m_windowRef));
    RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(mouseEvent, EventTypeId, m_mouseToEventNameRefMap[type]));

    if (type == HoloJs::MouseButtonEventType::Wheel) {
        // The delta value comes in the button parameter
        RETURN_IF_FAILED(ScriptHostUtilities::SetJsProperty(mouseEvent, deltaYId, button));
        RETURN_IF_FAILED(invokeEventListeners(L"wheel", mouseEvent));
    } else {
        RETURN_IF_FAILED(invokeEventListeners(matchingEventName->second, mouseEvent));
    }

    return S_OK;
}