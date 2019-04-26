#pragma once

#include <memory>

namespace HoloJs {
namespace MixedReality {

class DeviceResources;    

class CameraResources {
   public:
    CameraResources(Windows::Graphics::Holographic::HolographicCamera ^ holographicCamera);

    HRESULT createResourcesForBackBuffer(DeviceResources* pDeviceResources,
                                      Windows::Graphics::Holographic::HolographicCameraRenderingParameters ^
                                          cameraParameters);
    void releaseResourcesForBackBuffer(DeviceResources* pDeviceResources);

    void UpdateViewProjectionBuffer(std::shared_ptr<DeviceResources> deviceResources,
                                    Windows::Graphics::Holographic::HolographicCameraPose ^ cameraPose,
                                    Windows::Perception::Spatial::SpatialCoordinateSystem ^ coordinateSystem);

    // Direct3D device resources.
    ID3D11RenderTargetView* GetBackBufferRenderTargetView() const { return m_d3dRenderTargetView.Get(); }

    ID3D11Texture2D* GetBackBufferTexture2D() const { return m_d3dBackBuffer.Get(); }
    D3D11_VIEWPORT GetViewport() const { return m_d3dViewport; }
    DXGI_FORMAT GetBackBufferDXGIFormat() const { return m_dxgiFormat; }

    // Render target properties.
    Windows::Foundation::Size GetRenderTargetSize() const { return m_d3dRenderTargetSize; }
    bool IsRenderingStereoscopic() const { return m_isStereo; }

    // The holographic camera these resources are for.
    Windows::Graphics::Holographic::HolographicCamera ^ GetHolographicCamera() const { return m_holographicCamera; }

   private:
    // Direct3D rendering objects. Required for 3D.
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_d3dRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_d3dBackBuffer;

    // Direct3D rendering properties.
    DXGI_FORMAT m_dxgiFormat;
    Windows::Foundation::Size m_d3dRenderTargetSize;
    D3D11_VIEWPORT m_d3dViewport;

    // Indicates whether the camera supports stereoscopic rendering.
    bool m_isStereo = false;

    // Indicates whether this camera has a pending frame.
    bool m_framePending = false;

    // Pointer to the holographic camera these resources are for.
    Windows::Graphics::Holographic::HolographicCamera ^ m_holographicCamera = nullptr;
};


}  // namespace MixedReality
}  // namespace HoloJs
