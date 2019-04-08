#include "stdafx.h"
#include "include/holojs/windows/mixed-reality/camera-resources.h"
#include "include/holojs/windows/mixed-reality/device-resources.h"
#include <windows.graphics.directx.direct3d11.interop.h>
#include "holojs/private/error-handling.h"

using namespace HoloJs::MixedReality;
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Graphics::DirectX::Direct3D11;
using namespace Windows::Graphics::Holographic;
using namespace Windows::Perception::Spatial;
using namespace std;

CameraResources::CameraResources(HolographicCamera ^ camera)
    : m_holographicCamera(camera), m_isStereo(camera->IsStereo), m_d3dRenderTargetSize(camera->RenderTargetSize)
{
    // TODO: expose these to the script and make them modifiable?
    camera->SetFarPlaneDistance(1000);
    camera->SetNearPlaneDistance(0.1);
    m_d3dViewport = CD3D11_VIEWPORT(0.f, 0.f, m_d3dRenderTargetSize.Width, m_d3dRenderTargetSize.Height);
};

HRESULT CameraResources::createResourcesForBackBuffer(DeviceResources* pDeviceResources,
                                                       HolographicCameraRenderingParameters ^ cameraParameters)
{
    const auto device = pDeviceResources->getD3DDevice();

    // Get the WinRT object representing the holographic camera's back buffer.
    IDirect3DSurface ^ surface = cameraParameters->Direct3D11BackBuffer;

    // Get a DXGI interface for the holographic camera's back buffer.
    // Holographic cameras do not provide the DXGI swap chain, which is owned
    // by the system. The Direct3D back buffer resource is provided using WinRT
    // interop APIs.
    ComPtr<ID3D11Resource> resource;
    RETURN_IF_FAILED(GetDXGIInterfaceFromObject(surface, IID_PPV_ARGS(&resource)));

    // Get a Direct3D interface for the holographic camera's back buffer.
    ComPtr<ID3D11Texture2D> cameraBackBuffer;
    RETURN_IF_FAILED(resource.As(&cameraBackBuffer));

    // Determine if the back buffer has changed. If so, ensure that the render target view
    // is for the current back buffer.
    if (m_d3dBackBuffer.Get() != cameraBackBuffer.Get()) {
        // This can change every frame as the system moves to the next buffer in the
        // swap chain. This mode of operation will occur when certain rendering modes
        // are activated.
        m_d3dBackBuffer = cameraBackBuffer;

        // Create a render target view of the back buffer.
        // Creating this resource is inexpensive, and is better than keeping track of
        // the back buffers in order to pre-allocate render target views for each one.
        RETURN_IF_FAILED(device->CreateRenderTargetView(m_d3dBackBuffer.Get(), nullptr, &m_d3dRenderTargetView));

        // Get the DXGI format for the back buffer.
        // This information can be accessed by the app using CameraResources::GetBackBufferDXGIFormat().
        D3D11_TEXTURE2D_DESC backBufferDesc;
        m_d3dBackBuffer->GetDesc(&backBufferDesc);
        m_dxgiFormat = backBufferDesc.Format;

        // Check for render target size changes.
        Windows::Foundation::Size currentSize = m_holographicCamera->RenderTargetSize;
        if (m_d3dRenderTargetSize != currentSize) {
            // Set render target size.
            m_d3dRenderTargetSize = currentSize;
        }
    }

    return S_OK;
}

// Releases resources associated with a back buffer.
void CameraResources::releaseResourcesForBackBuffer(DeviceResources* pDeviceResources)
{
    const auto context = pDeviceResources->getD3DDeviceContext();

    // Release camera-specific resources.
    m_d3dBackBuffer.Reset();
    m_d3dRenderTargetView.Reset();

    // Ensure system references to the back buffer are released by clearing the render
    // target from the graphics pipeline state, and then flushing the Direct3D context.
    ID3D11RenderTargetView* nullViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {nullptr};
    context->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
    context->Flush();
}

// Updates the view/projection constant buffer for a holographic camera.
void CameraResources::UpdateViewProjectionBuffer(shared_ptr<DeviceResources> deviceResources,
                                                     HolographicCameraPose ^ cameraPose,
                                                     SpatialCoordinateSystem ^ coordinateSystem)
{
    m_d3dViewport = CD3D11_VIEWPORT(
        cameraPose->Viewport.Left, cameraPose->Viewport.Top, cameraPose->Viewport.Width, cameraPose->Viewport.Height);
    HolographicStereoTransform cameraProjectionTransform = cameraPose->ProjectionTransform;

    Platform::IBox<HolographicStereoTransform> ^ viewTransformContainer =
        cameraPose->TryGetViewTransform(coordinateSystem);

    bool viewTransformAcquired = viewTransformContainer != nullptr;
    if (viewTransformAcquired) {
        // Otherwise, the set of view transforms can be retrieved.
        HolographicStereoTransform viewCoordinateSystemTransform = viewTransformContainer->Value;
    }
}