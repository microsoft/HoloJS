#pragma once

#include "chakra.h"
#include <GLES2/gl2.h>
#include <string>

namespace HoloJs {

class ScriptHostUtilities {
   public:
    static long ProjectFunction(const std::wstring& name,
                                const std::wstring& namespaceName,
                                JsNativeFunction function,
                                void* callbackState,
                                JsValueRef* scriptFunction);

    static long ProjectFunction(const std::wstring& name, const std::wstring& namespaceName, JsNativeFunction function)
    {
        JsValueRef unusedScriptFunction;
        return ProjectFunction(name, namespaceName, function, nullptr, &unusedScriptFunction);
    }

    static long GetOrCreateJsProperty(JsValueRef& parentObject, const std::wstring& name, JsValueRef* createdProperty);
    static long GetJsProperty(JsValueRef& parentObject, const std::wstring& name, JsValueRef* createdProperty);

    static long GetNativeInterfaceJsProperty(JsValueRef* createdProperty);

    static long SetJsProperty(JsValueRef& parentObject, const std::wstring& propertyName, JsValueRef& propertyValue);

    static long SetJsProperty(JsValueRef& parentObject, const std::wstring& propertyName, int intProperty);
    static long SetJsProperty(JsValueRef& parentObject, const std::wstring& propertyName, double doubleProperty);
    static long SetJsProperty(JsValueRef& parentObject,
                              const std::wstring& propertyName,
                              const std::wstring& stringProperty);

    static long GetString(JsValueRef value, std::wstring& outString);

    static long GetTypedArrayBufferFromRef(JsTypedArrayType type,
                                           JsValueRef typedArray,
                                           unsigned int* elementCount,
                                           unsigned char** nativeBuffer);

    static long CreateArrayBufferFromBuffer(JsValueRef* arrayBuffer,
                                            unsigned char* nativeBuffer,
                                            unsigned int nativeBufferLength);

    static long CreateTypedArray(JsTypedArrayType type,
                                 JsValueRef* arrayBuffer,
                                 unsigned int elementCount,
                                 unsigned char** nativeBuffer);

    static long CreateTypedArrayFromBuffer(JsTypedArrayType type,
                                           JsValueRef* arrayBuffer,
                                           unsigned int elementCount,
                                           unsigned char* source);

    static long CreateArrayBuffer(JsValueRef* arrayBuffer, unsigned int size, unsigned char** nativeBuffer);

    static long SetFloat32ArrayProperty(unsigned int elementCount,
                                        JsValueRef* jsRef,
                                        float** storagePointer,
                                        JsValueRef parentObject,
                                        PCWSTR referenceName);

    static long SetFloat64ArrayProperty(unsigned int elementCount,
                                        JsValueRef* jsRef,
                                        double** storagePointer,
                                        JsValueRef parentObject,
                                        PCWSTR referenceName);

    static long SetTypedArrayProperty(unsigned int elementCount,
                                      JsValueRef* jsRef,
                                      void** storagePointer,
                                      JsValueRef parentObject,
                                      PCWSTR referenceName,
                                      JsTypedArrayType arrayType);

    static GLclampf GLclampfFromJsRef(JsValueRef value);

    static GLenum GLenumFromJsRef(JsValueRef value);

    static GLint GLintFromJsRef(JsValueRef value);

    static GLboolean GLbooleanFromJsRef(JsValueRef value);

    static GLbitfield GLbitfieldFromJsRef(JsValueRef value);

    static GLsizeiptr GLsizeiptrFromJsRef(JsValueRef value);

    static GLuint GLuintFromJsRef(JsValueRef value);

    static GLsizei GLsizeiFromJsRef(JsValueRef value);

    static GLfloat GLfloatFromJsRef(JsValueRef value);
    static double doubleFromJsRef(JsValueRef value);
};

}  // namespace HoloJs
