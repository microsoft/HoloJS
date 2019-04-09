#include "blob.h"
#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/script-host-utilities.h"
#include "resource-management/resource-manager.h"

using namespace HoloJs::Interfaces;
using namespace HoloJs::ResourceManagement;
using namespace std;

long BlobProjection::initialize()
{
    JS_PROJECTION_REGISTER(L"blob", L"create", create);
    return S_OK;
}

JsValueRef BlobProjection::_create(JsValueRef* arguments, unsigned short argumentCount)
{
    static std::wstring invalidDataException = L"invalid data passed to blob constructor";
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto nativeBlob = Blob::fromScriptData(arguments[1], arguments[2], m_resourceManager);

    if (nativeBlob == nullptr) {
        JsValueRef exceptionRef;

        RETURN_INVALID_REF_IF_JS_ERROR(
            JsPointerToString(invalidDataException.c_str(), invalidDataException.length(), &exceptionRef));
        RETURN_INVALID_REF_IF_JS_ERROR(JsSetException(exceptionRef));

        return JS_INVALID_REFERENCE;
    }

    RETURN_INVALID_REF_IF_NULL(nativeBlob);

    JsValueRef blobRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsCreateObject(&blobRef));

    auto nativeBlobRef = m_resourceManager->objectToDirectExternal(nativeBlob, ObjectType::Blob);

    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(blobRef, L"native", nativeBlobRef));
    RETURN_INVALID_REF_IF_FAILED(
        ScriptHostUtilities::SetJsProperty(blobRef, L"size", static_cast<int>(nativeBlob->data()->size())));
    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(blobRef, L"type", nativeBlob->mimeType()));

    return blobRef;
}

Blob* Blob::fromScriptData(JsValueRef blobParts,
                           JsValueRef options,
                           std::shared_ptr<HoloJs::ResourceManagement::ResourceManager> resourceManager)
{
    JsValueType partsType;
    JsValueType optionsType;

    RETURN_NULL_IF_JS_ERROR(JsGetValueType(blobParts, &partsType));
    RETURN_NULL_IF_JS_ERROR(JsGetValueType(options, &optionsType));

    RETURN_NULL_IF_FALSE(partsType == JsArray);

    auto blob = make_unique<Blob>();

    if (optionsType == JsObject) {
        JsValueRef typeRef;
        if (HOLOJS_SUCCEEDED(ScriptHostUtilities::GetJsProperty(options, L"type", &typeRef))) {
            RETURN_NULL_IF_FAILED(ScriptHostUtilities::GetString(typeRef, blob->m_mimeType));
        } else {
            blob->m_mimeType = L"";
        }
    }

    // Get the length of the parts array
    JsValueRef lengthRef;
    RETURN_NULL_IF_FAILED(ScriptHostUtilities::GetJsProperty(blobParts, L"length", &lengthRef));
    int length = ScriptHostUtilities::GLintFromJsRef(lengthRef);

    vector<unsigned char*> partsData(length);
    vector<unsigned int> partsDataLength(length);
    vector<std::vector<unsigned char>> stringParts;
    size_t totalSize = 0;

    // Enumerate all the parts, sum theirs sizes and obtain pointers to their data
    for (int i = 0; i < length; i++) {
        JsValueRef indexRef;
        RETURN_NULL_IF_JS_ERROR(JsIntToNumber(i, &indexRef));

        JsValueRef partRef;
        RETURN_NULL_IF_JS_ERROR(JsGetIndexedProperty(blobParts, indexRef, &partRef));

        JsValueType partType;
        RETURN_NULL_IF_JS_ERROR(JsGetValueType(partRef, &partType));

        RETURN_NULL_IF_FALSE(
            partType == JsString || partType == JsObject || partType == JsArrayBuffer || partType == JsTypedArray ||
            partType == JsArrayTypeFloat32 || partType == JsArrayTypeFloat64 || partType == JsArrayTypeInt16 ||
            partType == JsArrayTypeInt32 || partType == JsArrayTypeInt8 || partType == JsArrayTypeUint16 ||
            partType == JsArrayTypeUint32 || partType == JsArrayTypeUint8 || partType == JsArrayTypeUint8Clamped);

        if (partType == JsObject) {
            JsValueRef blobNativeRef;
            RETURN_NULL_IF_FAILED(ScriptHostUtilities::GetJsProperty(partRef, L"native", &blobNativeRef));

            auto blobPart = resourceManager->externalToObject<Blob>(blobNativeRef, ObjectType::Blob);
            RETURN_NULL_IF_FALSE(blobPart != nullptr);
            partsData[i] = blobPart->m_data->data();
			RETURN_NULL_IF_TRUE(blobPart->m_data->size() > UINT32_MAX);
            partsDataLength[i] = static_cast<unsigned int>(blobPart->m_data->size());
        } else if (partType == JsTypedArray) {
            RETURN_NULL_IF_JS_ERROR(
                JsGetTypedArrayStorage(partRef, &partsData[i], &partsDataLength[i], nullptr, nullptr));
        } else if (partType == JsString) {
            wstring stringData;
            RETURN_NULL_IF_FAILED(ScriptHostUtilities::GetString(partRef, stringData));

            stringParts.emplace_back(stringData.length() * sizeof(wchar_t));

            memcpy(stringParts.back().data(), stringData.c_str(), stringData.length());
			RETURN_NULL_IF_TRUE(stringParts.back().size() > UINT32_MAX);
            partsDataLength[i] = static_cast<unsigned int>(stringParts.back().size());
            partsData[i] = stringParts.back().data();
        } else {
            RETURN_NULL_IF_JS_ERROR(JsGetArrayBufferStorage(partRef, &partsData[i], &partsDataLength[i]));
        }

        totalSize += partsDataLength[i];
    }

    blob->m_data = make_shared<vector<unsigned char>>(totalSize);

    // Now concatenate all the parts into the blob buffer
    size_t appendOffset = 0;
    for (int i = 0; i < length; i++) {
        memcpy(blob->m_data->data() + appendOffset, partsData[i], partsDataLength[i]);
		// TODO: safe add
        appendOffset += partsDataLength[i];
    }

    return blob.release();
}