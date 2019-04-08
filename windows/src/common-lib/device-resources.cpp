#include "stdafx.h"
#include "include/holojs/windows/mixed-reality/camera-resources.h"
#include "include/holojs/windows/mixed-reality/device-resources.h"
#include <Collection.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include "holojs/private/error-handling.h"

using namespace D2D1;
using namespace Microsoft::WRL;
using namespace Windows::Graphics::DirectX::Direct3D11;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Holographic;

using namespace HoloJs::MixedReality;

// Constructor for DeviceResources.
DeviceResources::DeviceResources() {}

HRESULT DeviceResources::initialize(IHolographicSpace ^ holographicSpace)
{
    LUID id = {holographicSpace->PrimaryAdapterId.LowPart, holographicSpace->PrimaryAdapterId.HighPart};

    if ((id.HighPart != 0) || (id.LowPart != 0)) {
        UINT createFlags = 0;
#ifdef DEBUG
        if (SdkLayersAvailable()) {
            createFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
#endif

        ComPtr<IDXGIFactory1> dxgiFactory;
        RETURN_IF_FAILED(CreateDXGIFactory2(createFlags, IID_PPV_ARGS(&dxgiFactory)));

        ComPtr<IDXGIFactory4> dxgiFactory4;
        RETURN_IF_FAILED(dxgiFactory.As(&dxgiFactory4));

        // Retrieve the adapter specified by the holographic space.
        RETURN_IF_FAILED(dxgiFactory4->EnumAdapterByLuid(id, IID_PPV_ARGS(&m_dxgiAdapter)));
    } else {
        m_dxgiAdapter.Reset();
    }

    RETURN_IF_FAILED(createDeviceResources());

    holographicSpace->SetDirect3D11Device(m_d3dInteropDevice);

    return S_OK;
}

// Configures the Direct3D device, and stores handles to it and the device context.
HRESULT DeviceResources::createDeviceResources()
{
    // This flag adds support for surfaces with a different color channel ordering
    // than the API default. It is required for compatibility with Direct2D.
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // This array defines the set of DirectX hardware feature levels this app will support.
    // Note the ordering should be preserved.
    // Note that HoloLens supports feature level 11.1. The HoloLens emulator is also capable
    // of running on graphics cards starting with feature level 10.0.
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};

    // Create the Direct3D 11 API device object and a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    RETURN_IF_FAILED(D3D11CreateDevice(
        m_dxgiAdapter.Get(),  // Either nullptr, or the primary adapter determined by Windows Holographic.
        m_dxgiAdapter ? D3D_DRIVER_TYPE_UNKNOWN
                      : D3D_DRIVER_TYPE_HARDWARE,  // Create a device using the hardware graphics driver.
        0,                                         // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        creationFlags,                             // Set debug and Direct2D compatibility flags.
        featureLevels,                             // List of feature levels this app can support.
        ARRAYSIZE(featureLevels),                  // Size of the list above.
        D3D11_SDK_VERSION,                         // Always set this to D3D11_SDK_VERSION for Windows Runtime apps.
        &device,                                   // Returns the Direct3D device created.
        &m_d3dFeatureLevel,                        // Returns feature level of device created.
        &context                                   // Returns the device immediate context.
        ));

    // Store pointers to the Direct3D device and immediate context.
    RETURN_IF_FAILED(device.As(&m_d3dDevice));

    RETURN_IF_FAILED(context.As(&m_d3dContext));

    // Acquire the DXGI interface for the Direct3D device.
    ComPtr<IDXGIDevice3> dxgiDevice;
    RETURN_IF_FAILED(m_d3dDevice.As(&dxgiDevice));

    // Wrap the native device using a WinRT interop object.
    m_d3dInteropDevice = CreateDirect3DDevice(dxgiDevice.Get());

    // Cache the DXGI adapter.
    // This is for the case of no preferred DXGI adapter, or fallback to WARP.
    ComPtr<IDXGIAdapter> dxgiAdapter;
    RETURN_IF_FAILED(dxgiDevice->GetAdapter(&dxgiAdapter));
    RETURN_IF_FAILED(dxgiAdapter.As(&m_dxgiAdapter));

    return S_OK;
}

// Validates the back buffer for each HolographicCamera and recreates
// resources for back buffers that have changed.
// Locks the set of holographic camera resources until the function exits.
void DeviceResources::ensureCameraResources(HolographicFrame ^ frame, HolographicFramePrediction ^ prediction)
{
    useHolographicCameraResources<void>(
        [this, frame, prediction](std::map<UINT32, std::unique_ptr<CameraResources>>& cameraResourceMap) {
            for (const auto& pose : prediction->CameraPoses) {
                HolographicCameraRenderingParameters ^ renderingParameters = frame->GetRenderingParameters(pose);
                CameraResources* pCameraResources = cameraResourceMap[pose->HolographicCamera->Id].get();

                pCameraResources->createResourcesForBackBuffer(this, renderingParameters);
            }
        });
}

// Prepares to allocate resources and adds resource views for a camera.
// Locks the set of holographic camera resources until the function exits.
void DeviceResources::addHolographicCamera(HolographicCamera ^ camera)
{
    useHolographicCameraResources<void>(
        [this, camera](std::map<UINT32, std::unique_ptr<CameraResources>>& cameraResourceMap) {
            cameraResourceMap[camera->Id] = std::make_unique<CameraResources>(camera);
        });
}

// Deallocates resources for a camera and removes the camera from the set.
// Locks the set of holographic camera resources until the function exits.
void DeviceResources::removeHolographicCamera(HolographicCamera ^ camera)
{
    useHolographicCameraResources<void>(
        [this, camera](std::map<UINT32, std::unique_ptr<CameraResources>>& cameraResourceMap) {
            CameraResources* pCameraResources = cameraResourceMap[camera->Id].get();

            if (pCameraResources != nullptr) {
                pCameraResources->releaseResourcesForBackBuffer(this);
                cameraResourceMap.erase(camera->Id);
            }
        });
}

// Recreate all device resources and set them back to the current state.
// Locks the set of holographic camera resources until the function exits.
void DeviceResources::handleDeviceLost()
{
    if (m_deviceNotify != nullptr) {
        m_deviceNotify->OnDeviceLost();
    }

    useHolographicCameraResources<void>([this](std::map<UINT32, std::unique_ptr<CameraResources>>& cameraResourceMap) {
        for (auto& pair : cameraResourceMap) {
            CameraResources* pCameraResources = pair.second.get();
            pCameraResources->releaseResourcesForBackBuffer(this);
        }
    });

    if (m_deviceNotify != nullptr) {
        m_deviceNotify->OnDeviceRestored();
    }
}

// Register our DeviceNotify to be informed on device lost and creation.
void DeviceResources::registerDeviceNotify(IDeviceNotify* deviceNotify) { m_deviceNotify = deviceNotify; }

// Call this method when the app suspends. It provides a hint to the driver that the app
// is entering an idle state and that temporary buffers can be reclaimed for use by other apps.
HRESULT DeviceResources::trim()
{
    m_d3dContext->ClearState();

    ComPtr<IDXGIDevice3> dxgiDevice;
    RETURN_IF_FAILED(m_d3dDevice.As(&dxgiDevice));
    dxgiDevice->Trim();

    return S_OK;
}

// Present the contents of the swap chain to the screen.
// Locks the set of holographic camera resources until the function exits.
void DeviceResources::present(HolographicFrame ^ frame)
{
    // By default, this API waits for the frame to finish before it returns.
    // Holographic apps should wait for the previous frame to finish before
    // starting work on a new frame. This allows for better results from
    // holographic frame predictions.
    HolographicFramePresentResult presentResult = frame->PresentUsingCurrentPrediction();

    // The PresentUsingCurrentPrediction API will detect when the graphics device
    // changes or becomes invalid. When this happens, it is considered a Direct3D
    // device lost scenario.
    if (presentResult == HolographicFramePresentResult::DeviceRemoved) {
        // The Direct3D device, context, and resources should be recreated.
        handleDeviceLost();
    }
}