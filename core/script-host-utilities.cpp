#include "include/holojs/private/script-host-utilities.h"
#include "host-interfaces.h"

using namespace HoloJs;
using namespace std;

wstring HoloJsNavigatorPropertyName = L"holojs";
wstring NativeInterfacePropertyName = L"nativeInterface";

long ScriptHostUtilities::SetJsProperty(JsValueRef& parentObject,
                                        const std::wstring& propertyName,
                                        JsValueRef& propertyValue)
{
    JsPropertyIdRef propertyId;
    RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(propertyName.c_str(), &propertyId));
    RETURN_IF_JS_ERROR(JsSetProperty(parentObject, propertyId, propertyValue, true));

    return S_OK;
}

long ScriptHostUtilities::SetJsProperty(JsValueRef& parentObject, const std::wstring& propertyName, int intProperty)
{
    JsValueRef propertyRef;
    RETURN_IF_JS_ERROR(JsIntToNumber(intProperty, &propertyRef));
    return SetJsProperty(parentObject, propertyName, propertyRef);
}

long ScriptHostUtilities::SetJsProperty(JsValueRef& parentObject, const std::wstring& propertyName, double doubleProperty)
{
    JsValueRef propertyRef;
    RETURN_IF_JS_ERROR(JsDoubleToNumber(doubleProperty, &propertyRef));
    return SetJsProperty(parentObject, propertyName, propertyRef);
}


long ScriptHostUtilities::SetJsProperty(JsValueRef& parentObject,
                                        const std::wstring& propertyName,
                                        const std::wstring& stringProperty)
{
    JsValueRef propertyRef;
    RETURN_IF_JS_ERROR(JsPointerToString(stringProperty.c_str(), stringProperty.length(), &propertyRef));
    return SetJsProperty(parentObject, propertyName, propertyRef);
}

long ScriptHostUtilities::GetOrCreateJsProperty(JsValueRef& parentObject,
                                                const std::wstring& name,
                                                JsValueRef* outProperty)
{
    JsPropertyIdRef propertyId;
    RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(name.c_str(), &propertyId));

    // Create or get parent.name
    bool hasProperty;
    JsValueRef property;
    RETURN_IF_JS_ERROR(JsHasProperty(parentObject, propertyId, &hasProperty));
    if (!hasProperty) {
        RETURN_IF_JS_ERROR(JsCreateObject(&property));
        RETURN_IF_JS_ERROR(JsSetProperty(parentObject, propertyId, property, true));
    } else {
        RETURN_IF_JS_ERROR(JsGetProperty(parentObject, propertyId, &property));
    }

    *outProperty = property;

    return S_OK;
}

long ScriptHostUtilities::GetJsProperty(JsValueRef& parentObject, const std::wstring& name, JsValueRef* outProperty)
{
    JsPropertyIdRef propertyId;
    RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(name.c_str(), &propertyId));

    // Create or get parent.name
    bool hasProperty;
    JsValueRef property;
    RETURN_IF_JS_ERROR(JsHasProperty(parentObject, propertyId, &hasProperty));

    if (hasProperty) {
        RETURN_IF_JS_ERROR(JsGetProperty(parentObject, propertyId, &property));

        *outProperty = property;

        return S_OK;
    } else {
        return E_NOT_SET;
    }
}

long ScriptHostUtilities::GetNativeInterfaceJsProperty(JsValueRef* nativeInterfaceRef)
{
    JsValueRef globalObject;
    RETURN_IF_JS_ERROR(JsGetGlobalObject(&globalObject));

    JsValueRef navigator;
    RETURN_IF_FAILED(GetOrCreateJsProperty(globalObject, L"navigator", &navigator));

    JsValueRef holojs;
    RETURN_IF_FAILED(GetOrCreateJsProperty(navigator, HoloJsNavigatorPropertyName, &holojs));

    RETURN_IF_FAILED(GetOrCreateJsProperty(holojs, NativeInterfacePropertyName, nativeInterfaceRef));

    return S_OK;
}

long ScriptHostUtilities::ProjectFunction(const std::wstring& name,
                                          const std::wstring& namespaceName,
                                          JsNativeFunction nativeFunction,
                                          void* callbackState,
                                          JsValueRef* scriptFunction)
{
    // Create or get the native interface property
    JsValueRef nativeInterface;
    RETURN_IF_FAILED(GetNativeInterfaceJsProperty(&nativeInterface));

    // Create or get navigator.holojs.nativeInterface.[namespace]
    JsValueRef namespaceProperty;
    RETURN_IF_FAILED(GetOrCreateJsProperty(nativeInterface, namespaceName, &namespaceProperty));

    JsPropertyIdRef functionPropertyId;
    RETURN_IF_JS_ERROR(JsGetPropertyIdFromName(name.c_str(), &functionPropertyId));
    RETURN_IF_JS_ERROR(JsCreateFunction(nativeFunction, callbackState, scriptFunction));
    RETURN_IF_JS_ERROR(JsSetProperty(namespaceProperty, functionPropertyId, *scriptFunction, true));

    return S_OK;
}

long ScriptHostUtilities::GetString(JsValueRef value, wstring& outString)
{
    PCWSTR rawString;
    size_t rawStringLength;
    RETURN_IF_JS_ERROR(JsStringToPointer(value, &rawString, &rawStringLength));

    outString.assign(rawString);

    return S_OK;
}

long ScriptHostUtilities::GetTypedArrayBufferFromRef(JsTypedArrayType desiredType,
                                           JsValueRef typedArrayRef,
                                           unsigned int* elementCount,
                                           unsigned char** nativeBuffer)
{
    JsValueType refType;
    RETURN_IF_JS_ERROR(JsGetValueType(typedArrayRef, &refType));
    RETURN_IF_FALSE(refType == JsTypedArray);

    JsTypedArrayType actualType;
    RETURN_IF_JS_ERROR(
        JsGetTypedArrayInfo(typedArrayRef, &actualType, nullptr, nullptr, nullptr));
    RETURN_IF_FALSE(actualType == desiredType);

    unsigned int typedBufferLength;
    int typedBufferElementSize;
    JsTypedArrayType typeArrayType;
    RETURN_IF_JS_ERROR(JsGetTypedArrayStorage(
        typedArrayRef, nativeBuffer, &typedBufferLength, &typeArrayType, &typedBufferElementSize));

    *elementCount = typedBufferLength / typedBufferElementSize;

    return S_OK;
}

long ScriptHostUtilities::CreateTypedArray(JsTypedArrayType type,
                                           JsValueRef* typedArray,
                                           unsigned int elementCount,
                                           unsigned char** nativeBuffer)
{
    RETURN_IF_JS_ERROR(JsCreateTypedArray(type, JS_INVALID_REFERENCE, 0, elementCount, typedArray));

    unsigned int typedBufferLength;
    int typedBufferElementSize;
    JsTypedArrayType typeArrayType;
    RETURN_IF_JS_ERROR(
        JsGetTypedArrayStorage(*typedArray, nativeBuffer, &typedBufferLength, &typeArrayType, &typedBufferElementSize));

    RETURN_IF_TRUE(typedBufferLength != elementCount * typedBufferElementSize);

    return S_OK;
}

long ScriptHostUtilities::CreateTypedArrayFromBuffer(JsTypedArrayType type,
                                                     JsValueRef* typedArray,
                                                     unsigned int elementCount,
                                                     unsigned char* source)
{
    RETURN_IF_JS_ERROR(JsCreateTypedArray(type, JS_INVALID_REFERENCE, 0, elementCount, typedArray));

    unsigned int typedBufferLength;
    unsigned char* typedBufferPointer;
    int typedBufferElementSize;
    JsTypedArrayType typeArrayType;
    RETURN_IF_JS_ERROR(JsGetTypedArrayStorage(
        *typedArray, &typedBufferPointer, &typedBufferLength, &typeArrayType, &typedBufferElementSize));

    RETURN_IF_TRUE(typedBufferLength != elementCount * typedBufferElementSize);

    memcpy(typedBufferPointer, source, typedBufferLength);

    return S_OK;
}

long ScriptHostUtilities::CreateArrayBuffer(JsValueRef* arrayBuffer, unsigned int size, unsigned char** nativeBuffer)
{
    RETURN_IF_JS_ERROR(JsCreateArrayBuffer(size, arrayBuffer));

    unsigned int arrayBufferLength;
    RETURN_IF_JS_ERROR(JsGetArrayBufferStorage(*arrayBuffer, nativeBuffer, &arrayBufferLength));

    RETURN_IF_TRUE(arrayBufferLength != size);

    return S_OK;
}

long ScriptHostUtilities::CreateArrayBufferFromBuffer(JsValueRef* arrayBuffer,
                                                      unsigned char* nativeBuffer,
                                                      unsigned int nativeBufferLength)
{
    RETURN_IF_JS_ERROR(JsCreateArrayBuffer(nativeBufferLength, arrayBuffer));

    unsigned int arrayBufferLength;
    unsigned char* arrayBufferPointer;
    RETURN_IF_JS_ERROR(JsGetArrayBufferStorage(*arrayBuffer, &arrayBufferPointer, &arrayBufferLength));

    RETURN_IF_TRUE(arrayBufferLength != nativeBufferLength);

    memcpy(arrayBufferPointer, nativeBuffer, nativeBufferLength);

    return S_OK;
}

long ScriptHostUtilities::SetFloat32ArrayProperty(
    unsigned int elementCount, JsValueRef* jsRef, float** storagePointer, JsValueRef parentObject, PCWSTR referenceName)
{
    return SetTypedArrayProperty(elementCount,
                                 jsRef,
                                 reinterpret_cast<void**>(storagePointer),
                                 parentObject,
                                 referenceName,
                                 JsTypedArrayType::JsArrayTypeFloat32);
}

long ScriptHostUtilities::SetFloat64ArrayProperty(unsigned int elementCount,
                                                  JsValueRef* jsRef,
                                                  double** storagePointer,
                                                  JsValueRef parentObject,
                                                  PCWSTR referenceName)
{
    return SetTypedArrayProperty(elementCount,
                                 jsRef,
                                 reinterpret_cast<void**>(storagePointer),
                                 parentObject,
                                 referenceName,
                                 JsTypedArrayType::JsArrayTypeFloat64);
}

long ScriptHostUtilities::SetTypedArrayProperty(unsigned int elementCount,
                                                JsValueRef* jsRef,
                                                void** storagePointer,
                                                JsValueRef parentObject,
                                                PCWSTR referenceName,
                                                JsTypedArrayType arrayType)
{
    unsigned int bufferLength;
    JsTypedArrayType type;
    int elementSize;

    RETURN_IF_JS_ERROR(JsCreateTypedArray(arrayType, nullptr, 0, elementCount, jsRef));

    RETURN_IF_JS_ERROR(JsGetTypedArrayStorage(
        *jsRef, reinterpret_cast<ChakraBytePtr*>(storagePointer), &bufferLength, &type, &elementSize));

    RETURN_IF_JS_ERROR(ScriptHostUtilities::SetJsProperty(parentObject, referenceName, *jsRef));

    return S_OK;
}

GLclampf ScriptHostUtilities::GLclampfFromJsRef(JsValueRef value)
{
    double doubleValue;
    if (JsNumberToDouble(value, &doubleValue) != JsNoError) {
        return 0;
    }

    return (GLclampf)doubleValue;
}

GLenum ScriptHostUtilities::GLenumFromJsRef(JsValueRef value)
{
    int intValue;
    if (JsNumberToInt(value, &intValue) != JsNoError) {
        return 0;
    }

    return static_cast<unsigned int>(intValue);
}

GLint ScriptHostUtilities::GLintFromJsRef(JsValueRef value)
{
    int intValue;
    if (JsNumberToInt(value, &intValue) != JsNoError) {
        return 0;
    }

    return intValue;
}

GLboolean ScriptHostUtilities::GLbooleanFromJsRef(JsValueRef value)
{
    bool boolValue;
    if (JsBooleanToBool(value, &boolValue) != JsNoError) {
        return false;
    }

    return boolValue;
}

GLbitfield ScriptHostUtilities::GLbitfieldFromJsRef(JsValueRef value)
{
    return static_cast<GLbitfield>(GLintFromJsRef(value));
}

GLsizeiptr ScriptHostUtilities::GLsizeiptrFromJsRef(JsValueRef value)
{
    double doubleValue;
    if (JsNumberToDouble(value, &doubleValue) != JsNoError) {
        return 0;
    }

    return static_cast<GLsizeiptr>(doubleValue);
}

GLuint ScriptHostUtilities::GLuintFromJsRef(JsValueRef value) { return static_cast<GLuint>(GLintFromJsRef(value)); }

GLsizei ScriptHostUtilities::GLsizeiFromJsRef(JsValueRef value) { return GLintFromJsRef(value); }

double ScriptHostUtilities::doubleFromJsRef(JsValueRef value)
{
    double doubleValue;
    if (JsNumberToDouble(value, &doubleValue) != JsNoError) {
        return 0;
    }

    return doubleValue;
}

GLfloat ScriptHostUtilities::GLfloatFromJsRef(JsValueRef value) { return static_cast<GLfloat>(doubleFromJsRef(value)); }
