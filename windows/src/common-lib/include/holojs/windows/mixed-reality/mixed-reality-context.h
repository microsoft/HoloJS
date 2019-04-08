#pragma once

#include "../performance-tracker.h"
#include "device-resources.h"
#include "holojs/private/chakra.h"
#include "holojs/private/script-window.h"
#include "spatial-input.h"

namespace HoloJs {
namespace MixedReality {

class MixedRealityContext {
   public:
    MixedRealityContext() : m_presentPerformance(L"Present interval", 11) {}

    MixedRealityContext(const MixedRealityContext&) = delete;

    ~MixedRealityContext();

    HRESULT initializeHolographicSpace();
    HRESULT initializeScriptHost();
	void releaseScriptResources();

    Windows::Graphics::Holographic::HolographicFrame ^ createHolographicFrame();

    HRESULT render(Windows::Graphics::Holographic::HolographicFrame ^ frame, ID3D11Texture2D* content);

    void present(Windows::Graphics::Holographic::HolographicFrame ^ holographicFrame);

    static bool headsetAvailable() { return Windows::Graphics::Holographic::HolographicSpace::IsAvailable; }

    void setWindow(HWND window)
    {
        m_window = window;
    }
    void setCoreWindow(Windows::UI::Core::CoreWindow ^ window)
    {
        m_coreWindow = window;
    }

    Windows::Graphics::Holographic::IHolographicSpace ^ getHolographicSpace() { return m_holographicSpace; }

        float getHeight()
    {
        return m_renderTargetSize.Height;
    }

    float getWidth() { return m_renderTargetSize.Width; }

    bool isCameraAvailable() const { return m_cameraCount > 0; }

    std::shared_ptr<DeviceResources> getMixedRealityDeviceResources() { return m_deviceResources; }

    virtual HRESULT createHolographicSpace() = 0;

    void setScriptWindow(HoloJs::IWindow* window) { m_scriptWindow = window; }

    bool isWindingOrderReversed() const { return m_windingOrderReversed; }

	Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^ getStationaryFrameOfReference() { return m_stationaryFrameOfReference; }

   protected:
    HWND m_window;
    Platform::Agile<Windows::UI::Core::CoreWindow> m_coreWindow;
    Windows::Graphics::Holographic::IHolographicSpace ^ m_holographicSpace;

    HoloJs::IWindow* m_scriptWindow;

   private:
    std::shared_ptr<DeviceResources> m_deviceResources;

    // Asynchronously creates resources for new holographic cameras.
    void OnCameraAdded(Windows::Graphics::Holographic::HolographicSpace ^ sender,
                       Windows::Graphics::Holographic::HolographicSpaceCameraAddedEventArgs ^ args);

    // Synchronously releases resources for holographic cameras that are no longer
    // attached to the system.
    void OnCameraRemoved(Windows::Graphics::Holographic::HolographicSpace ^ sender,
                         Windows::Graphics::Holographic::HolographicSpaceCameraRemovedEventArgs ^ args);

    Windows::Foundation::EventRegistrationToken m_cameraAddedToken;
    Windows::Foundation::EventRegistrationToken m_cameraRemovedToken;

    Windows::Foundation::Size m_renderTargetSize;

    int m_cameraCount = 0;

    double* m_nearFarStoragePointer = nullptr;
    JsValueRef m_nearFarScriptProjection = JS_INVALID_REFERENCE;

    float* m_leftViewMatrixStoragePointer = nullptr;
    JsValueRef m_leftViewMatrixScriptProjection = JS_INVALID_REFERENCE;

    float* m_rightViewMatrixStoragePointer = nullptr;
    JsValueRef m_rightViewMatrixScriptProjection = JS_INVALID_REFERENCE;

    float* m_leftProjectionMatrixStoragePointer = nullptr;
    JsValueRef m_leftProjectionMatrixScriptProjection = JS_INVALID_REFERENCE;

    float* m_rightProjectionMatrixStoragePointer = nullptr;
    JsValueRef m_rightProjectionMatrixScriptProjection = JS_INVALID_REFERENCE;

    float* m_positionVectorStoragePointer = nullptr;
    JsValueRef m_positionVectorScriptProjection = JS_INVALID_REFERENCE;

    float* m_orientationVectorStoragePointer = nullptr;
    JsValueRef m_orientationVectorScriptProjection = JS_INVALID_REFERENCE;

    Windows::Perception::Spatial::SpatialLocator ^ m_spatialLocator;
    Windows::Perception::Spatial::SpatialStationaryFrameOfReference ^ m_stationaryFrameOfReference;
    Windows::Perception::Spatial::ISpatialLocatorAttachedFrameOfReference ^ m_attachedFrameOfReference;

    void updateProjectionMatrices(Windows::Graphics::Holographic::HolographicCameraPose ^ pose);

    void flipYViewTransform(Windows::Foundation::Numerics::float4x4* viewTransform, float* destination);

    static void setDepthMapping(Windows::Foundation::Numerics::float4x4& projectionMatrix,
                                double depthNear,
                                double depthFar);

	static DirectX::XMMATRIX getHeadTransform(Windows::UI::Input::Spatial::SpatialPointerPose ^ spatialPointerPose);

	static DirectX::XMMATRIX XMMatrixRTInverse(DirectX::FXMMATRIX M);

    PerformaceTracker m_presentPerformance;

    bool m_windingOrderReversed = true;
};

}  // namespace MixedReality
}  // namespace HoloJs
