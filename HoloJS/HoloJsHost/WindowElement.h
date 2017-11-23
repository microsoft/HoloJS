#pragma once

#include "KeyboardInput.h"
#include "MouseInput.h"
#include "SpatialInput.h"
#include "SpatialMapping.h"
#include "VoiceInput.h"

namespace HologramJS {
namespace API {

using float4x4 = Windows::Foundation::Numerics::float4x4;

class WindowElement {
   public:
    WindowElement();
    ~WindowElement();

    void Close();

    bool Initialize();

    void Resize(int width, int height);
    void VSync(float4x4 midViewMatrix,
               float4x4 midProjectionMatrix,
               float4x4 leftViewMatrix,
               float4x4 leftProjectionMatrix,
               float4x4 rightViewMatrix,
               float4x4 rightProjectionMatrix);

    Input::KeyboardInput& KeyboardRouter() { return m_keyboardInput; }
    Input::MouseInput& MouseRouter() { return m_mouseInput; }

    void SetBaseUrl(const std::wstring& baseUrl) { m_baseUrl.assign(baseUrl); }

    void SetStationaryFrameOfReference(Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^
                                       frameOfReference)
    {
        m_spatialInput.SetStationaryFrameOfReference(frameOfReference);
        m_spatialMapping.EnableSpatialMapping(frameOfReference);
    }

   private:
    std::wstring m_baseUrl;

    Input::KeyboardInput m_keyboardInput;
    Input::MouseInput m_mouseInput;
    Input::SpatialInput m_spatialInput;
    Input::SpatialMapping m_spatialMapping;
    Input::VoiceInput m_voiceInput;

    int m_width;
    int m_height;

    JsValueRef m_callbackFunction = JS_INVALID_REFERENCE;

    JsValueRef m_getWidthFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK getWidthStatic(JsValueRef callee,
                                                     bool isConstructCall,
                                                     JsValueRef* arguments,
                                                     unsigned short argumentCount,
                                                     PVOID callbackData)
    {
        return reinterpret_cast<WindowElement*>(callbackData)->getWidth(arguments, argumentCount);
    }

    JsValueRef getWidth(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getHeightFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK getHeightStatic(JsValueRef callee,
                                                      bool isConstructCall,
                                                      JsValueRef* arguments,
                                                      unsigned short argumentCount,
                                                      PVOID callbackData)
    {
        return reinterpret_cast<WindowElement*>(callbackData)->getHeight(arguments, argumentCount);
    }

    JsValueRef getHeight(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_getBaseUrlFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK getBaseUrlStatic(JsValueRef callee,
                                                       bool isConstructCall,
                                                       JsValueRef* arguments,
                                                       unsigned short argumentCount,
                                                       PVOID callbackData)
    {
        return reinterpret_cast<WindowElement*>(callbackData)->getBaseUrl(arguments, argumentCount);
    }

    JsValueRef getBaseUrl(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_setCallbackFunction = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK setCallbackStatic(JsValueRef callee,
                                                        bool isConstructCall,
                                                        JsValueRef* arguments,
                                                        unsigned short argumentCount,
                                                        PVOID callbackData)
    {
        return reinterpret_cast<WindowElement*>(callbackData)->setCallback(arguments, argumentCount);
    }

    JsValueRef setCallback(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_requestSpatialMapping = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK requestSpatialMappingStatic(JsValueRef callee,
                                                                  bool isConstructCall,
                                                                  JsValueRef* arguments,
                                                                  unsigned short argumentCount,
                                                                  PVOID callbackData)
    {
        return reinterpret_cast<WindowElement*>(callbackData)->requestSpatialMapping(arguments, argumentCount);
    }

    JsValueRef requestSpatialMapping(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_addEventListener = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK addEventListenerStatic(JsValueRef callee,
                                                             bool isConstructCall,
                                                             JsValueRef* arguments,
                                                             unsigned short argumentCount,
                                                             PVOID callbackData)
    {
        return reinterpret_cast<WindowElement*>(callbackData)->addEventListener(arguments, argumentCount);
    }
    JsValueRef addEventListener(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_removeEventListener = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK removeEventListenerStatic(JsValueRef callee,
                                                                bool isConstructCall,
                                                                JsValueRef* arguments,
                                                                unsigned short argumentCount,
                                                                PVOID callbackData)
    {
        return reinterpret_cast<WindowElement*>(callbackData)->removeEventListener(arguments, argumentCount);
    }
    JsValueRef removeEventListener(JsValueRef* arguments, unsigned short argumentCount);

    JsValueRef m_setVoiceCommands = JS_INVALID_REFERENCE;
    static JsValueRef CHAKRA_CALLBACK setVoiceCommandsStatic(JsValueRef callee,
                                                             bool isConstructCall,
                                                             JsValueRef* arguments,
                                                             unsigned short argumentCount,
                                                             PVOID callbackData)
    {
        return reinterpret_cast<WindowElement*>(callbackData)->setVoiceCommands(arguments, argumentCount);
    }
    JsValueRef setVoiceCommands(JsValueRef* arguments, unsigned short argumentCount);

    float* m_viewMatrixMidStoragePointer = nullptr;
    JsValueRef m_viewMatrixMidScriptProjection = JS_INVALID_REFERENCE;
    float* m_viewMatrixLeftStoragePointer = nullptr;
    JsValueRef m_viewMatrixLeftScriptProjection = JS_INVALID_REFERENCE;
    float* m_viewMatrixRightStoragePointer = nullptr;
    JsValueRef m_viewMatrixRightScriptProjection = JS_INVALID_REFERENCE;

    float* m_projectionMatrixMidStoragePointer = nullptr;
    JsValueRef m_projectionMatrixMidScriptProjection = JS_INVALID_REFERENCE;
    float* m_projectionMatrixLeftStoragePointer = nullptr;
    JsValueRef m_projectionMatrixLeftScriptProjection = JS_INVALID_REFERENCE;
    float* m_projectionMatrixRightStoragePointer = nullptr;
    JsValueRef m_projectionMatrixRightScriptProjection = JS_INVALID_REFERENCE;

    Windows::Foundation::Numerics::float4x4 m_inverseMidViewMatrix;
    Windows::Foundation::Numerics::float4x4 m_inverseLeftViewMatrix;
    Windows::Foundation::Numerics::float4x4 m_inverseRightViewMatrix;

    float* m_cameraPositionMidStoragePointer = nullptr;
    JsValueRef m_cameraPositionMidScriptProjection = JS_INVALID_REFERENCE;
    float* m_cameraPositionLeftStoragePointer = nullptr;
    JsValueRef m_cameraPositionLeftScriptProjection = JS_INVALID_REFERENCE;
    float* m_cameraPositionRightStoragePointer = nullptr;
    JsValueRef m_cameraPositionRightScriptProjection = JS_INVALID_REFERENCE;

    JsValueRef m_vsyncEventType;
    JsValueRef m_resizeEventType;

    bool CreateViewMatrixStorageAndScriptProjection();
};
}  // namespace API
}  // namespace HologramJS
