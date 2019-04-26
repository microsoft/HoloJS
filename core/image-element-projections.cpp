#include "include/holojs/private/script-host-utilities.h"
#include "include/holojs/private/image-interface.h"
#include "holojs-script-host.h"
#include "host-interfaces.h"
#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/platform-interfaces.h"
#include "resource-management/resource-manager.h"
#include "image-element-projections.h"
#include <functional>
#include <string>

using namespace HoloJs::Interfaces;
using namespace HoloJs::ResourceManagement;
using namespace std;

long ImageProjections::initialize()
{
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"create", L"image", staticCreateImage, this, &m_createImageFunction));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"setSource", L"image", staticSetSource, this, &m_setSourceFunction));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"setSourceFromDataUrl", L"image", staticSetSourceFromDataUrl, this, &m_setSourceFromDataUrlFunction));

    return S_OK;
}

JsValueRef ImageProjections::createImage(JsValueRef* arguments, unsigned short argumentCount)
{
    return m_resourceManager->objectToDirectExternal(getPlatform()->createImage(m_host), ObjectType::IImage);
}

JsValueRef ImageProjections::setSource(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);

    auto image = m_resourceManager->externalToObject<HoloJs::IIMage>(arguments[1], ObjectType::IImage);
    RETURN_INVALID_REF_IF_NULL(image);

    JsValueRef imageRef = arguments[2];

    wstring source;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(arguments[3], source));

    RETURN_INVALID_REF_IF_FAILED(image->setSource(source, imageRef));

    return JS_INVALID_REFERENCE;
}

JsValueRef ImageProjections::setSourceFromDataUrl(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 4);


    auto image = m_resourceManager->externalToObject<HoloJs::IIMage>(arguments[1], ObjectType::IImage);
    RETURN_INVALID_REF_IF_NULL(image);

    JsValueRef imageRef = arguments[2];

    JsValueRef blobRef = arguments[3];
    JsValueRef blobNativeRef;
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetJsProperty(blobRef, L"native", &blobNativeRef));

    auto blob = m_resourceManager->externalToObject<Blob>(blobNativeRef, ObjectType::Blob);
    RETURN_INVALID_REF_IF_FALSE(blob != nullptr);

    RETURN_INVALID_REF_IF_FAILED(image->setSourceFromBlob(blobRef, blob, imageRef));

    return JS_INVALID_REFERENCE;
}