#include "stdafx.h"
#include "../host-interfaces.h"
#include "holojs/private/script-host-utilities.h"
#include "include/holojs/windows/mixed-reality/mixed-reality-context.h"
#include <ppltasks.h>

using namespace HoloJs::MixedReality;
using namespace Windows::Graphics::Holographic;
using namespace Windows::Perception::Spatial;
using namespace Windows::UI::Input::Spatial;
using namespace std;
using namespace std::placeholders;
using namespace concurrency;
using namespace Windows::Foundation;
using namespace concurrency;
using namespace HoloJs::Interfaces;
using namespace DirectX;

MixedRealityContext::~MixedRealityContext()
{
    if (m_holographicSpace) {
        m_holographicSpace->CameraAdded -= m_cameraAddedToken;
        m_holographicSpace->CameraRemoved -= m_cameraRemovedToken;
    }
}

HRESULT MixedRealityContext::initializeHolographicSpace()
{
    m_spatialLocator = Windows::Perception::Spatial::SpatialLocator::GetDefault();

    auto position = Windows::Foundation::Numerics::float3();

    auto yAxis = Windows::Foundation::Numerics::float3();
    yAxis.z = 1.0;
    auto yAxisVector = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&yAxis));

    auto quaternionTest = XMQuaternionIdentity();
    auto orientationVector = XMQuaternionRotationNormal(yAxisVector, XM_PI);
    Windows::Foundation::Numerics::quaternion orientation;
    XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&orientation), orientationVector);
    orientation.x = -orientation.x;
    orientation.z = -orientation.z;

    m_stationaryFrameOfReference =
        m_spatialLocator->CreateStationaryFrameOfReferenceAtCurrentLocation();  // position, orientation, XM_PI);

    m_attachedFrameOfReference = m_spatialLocator->CreateAttachedFrameOfReferenceAtCurrentHeading();

    RETURN_IF_FAILED(createHolographicSpace());

    m_deviceResources = std::make_shared<DeviceResources>();
    m_deviceResources->initialize(m_holographicSpace);

    m_cameraAddedToken = m_holographicSpace->CameraAdded +=
        ref new Windows::Foundation::TypedEventHandler<HolographicSpace ^, HolographicSpaceCameraAddedEventArgs ^>(
            std::bind(&MixedRealityContext::OnCameraAdded, this, _1, _2));

    m_cameraRemovedToken = m_holographicSpace->CameraRemoved +=
        ref new Windows::Foundation::TypedEventHandler<HolographicSpace ^, HolographicSpaceCameraRemovedEventArgs ^>(
            std::bind(&MixedRealityContext::OnCameraRemoved, this, _1, _2));

    return S_OK;
}

HRESULT MixedRealityContext::initializeScriptHost()
{
    JsValueRef nativeInterface;
    RETURN_IF_FAILED(ScriptHostUtilities::GetNativeInterfaceJsProperty(&nativeInterface));

    RETURN_IF_FAILED(ScriptHostUtilities::SetFloat64ArrayProperty(
        2, &m_nearFarScriptProjection, &m_nearFarStoragePointer, nativeInterface, L"xrNearFar"));
    RETURN_IF_JS_ERROR(JsAddRef(m_nearFarScriptProjection, nullptr));

    m_nearFarStoragePointer[0] = 0.01;
    m_nearFarStoragePointer[1] = 10000.0;

    RETURN_IF_FAILED(ScriptHostUtilities::SetFloat32ArrayProperty(
        16, &m_leftViewMatrixScriptProjection, &m_leftViewMatrixStoragePointer, nativeInterface, L"xrLeftView"));
    RETURN_IF_JS_ERROR(JsAddRef(m_leftViewMatrixScriptProjection, nullptr));

    RETURN_IF_FAILED(ScriptHostUtilities::SetFloat32ArrayProperty(
        16, &m_rightViewMatrixScriptProjection, &m_rightViewMatrixStoragePointer, nativeInterface, L"xrRightView"));
    RETURN_IF_JS_ERROR(JsAddRef(m_rightViewMatrixScriptProjection, nullptr));

    RETURN_IF_FAILED(ScriptHostUtilities::SetFloat32ArrayProperty(16,
                                                                  &m_leftProjectionMatrixScriptProjection,
                                                                  &m_leftProjectionMatrixStoragePointer,
                                                                  nativeInterface,
                                                                  L"xrLeftProjection"));
    RETURN_IF_JS_ERROR(JsAddRef(m_leftProjectionMatrixScriptProjection, nullptr));

    RETURN_IF_FAILED(ScriptHostUtilities::SetFloat32ArrayProperty(16,
                                                                  &m_rightProjectionMatrixScriptProjection,
                                                                  &m_rightProjectionMatrixStoragePointer,
                                                                  nativeInterface,
                                                                  L"xrRightProjection"));
    RETURN_IF_JS_ERROR(JsAddRef(m_rightProjectionMatrixScriptProjection, nullptr));

    RETURN_IF_FAILED(ScriptHostUtilities::SetFloat32ArrayProperty(
        4, &m_positionVectorScriptProjection, &m_positionVectorStoragePointer, nativeInterface, L"xrPosition"));
    RETURN_IF_JS_ERROR(JsAddRef(m_positionVectorScriptProjection, nullptr));

    RETURN_IF_FAILED(ScriptHostUtilities::SetFloat32ArrayProperty(4,
                                                                  &m_orientationVectorScriptProjection,
                                                                  &m_orientationVectorStoragePointer,
                                                                  nativeInterface,
                                                                  L"xrOrientation"));
    RETURN_IF_JS_ERROR(JsAddRef(m_orientationVectorScriptProjection, nullptr));

    return S_OK;
}

void MixedRealityContext::releaseScriptResources()
{
    if (m_nearFarScriptProjection != JS_INVALID_REFERENCE) {
        JsRelease(m_nearFarScriptProjection, nullptr);
        m_nearFarScriptProjection = JS_INVALID_REFERENCE;
        m_nearFarStoragePointer = nullptr;
    }

    if (m_orientationVectorScriptProjection != JS_INVALID_REFERENCE) {
        JsRelease(m_orientationVectorScriptProjection, nullptr);
        m_orientationVectorScriptProjection = JS_INVALID_REFERENCE;
        m_orientationVectorStoragePointer = nullptr;
    }

    if (m_rightProjectionMatrixScriptProjection != JS_INVALID_REFERENCE) {
        m_rightProjectionMatrixScriptProjection = JS_INVALID_REFERENCE;
        JsRelease(m_rightProjectionMatrixScriptProjection, nullptr);
        m_rightProjectionMatrixStoragePointer = nullptr;
    }

    if (m_leftProjectionMatrixScriptProjection != JS_INVALID_REFERENCE) {
        JsRelease(m_leftProjectionMatrixScriptProjection, nullptr);
        m_leftProjectionMatrixScriptProjection = JS_INVALID_REFERENCE;
        m_leftProjectionMatrixStoragePointer = nullptr;
    }

    if (m_rightViewMatrixScriptProjection != JS_INVALID_REFERENCE) {
        JsRelease(m_rightViewMatrixScriptProjection, nullptr);
        m_rightViewMatrixScriptProjection = JS_INVALID_REFERENCE;
        m_rightViewMatrixStoragePointer = nullptr;
    }

    if (m_leftViewMatrixScriptProjection != JS_INVALID_REFERENCE) {
        JsRelease(m_leftViewMatrixScriptProjection, nullptr);
        m_leftViewMatrixScriptProjection = JS_INVALID_REFERENCE;
        m_leftViewMatrixStoragePointer = nullptr;
    }
}

DirectX::XMMATRIX MixedRealityContext::getHeadTransform(SpatialPointerPose ^ spatialPointerPose)
{
    // Get the components of the matrix
    auto headPose = spatialPointerPose->Head;

    auto forwardDirection = headPose->ForwardDirection;

    auto upDirection = headPose->UpDirection;

    auto position = headPose->Position;

    // Build the transform to the head
    return DirectX::XMMatrixLookToRH(DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&position)),
                                     DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&forwardDirection)),
                                     DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&upDirection)));
}

DirectX::XMMATRIX MixedRealityContext::XMMatrixRTInverse(DirectX::FXMMATRIX M)
{
    const XMVECTOR one = XMVectorSplatOne();

    XMMATRIX result = M;
    const XMVECTOR MTranslation = M.r[3];
    result.r[3] = XMVectorZero();
    result = XMMatrixTranspose(result);

    const XMVECTOR resultTranslation = -XMVector3TransformNormal(MTranslation, result);

    result.r[3] = XMVectorSelect(resultTranslation, one, XMVectorSelectControl(0, 0, 0, 1));
    return result;
}

HolographicFrame ^ MixedRealityContext::createHolographicFrame()
{
    RETURN_NULL_IF_FALSE(m_cameraCount > 0);

    auto holographicFrame = m_holographicSpace->CreateNextFrame();
    RETURN_NULL_IF_FALSE(holographicFrame != nullptr);

    holographicFrame->UpdateCurrentPrediction();
    auto prediction = holographicFrame->CurrentPrediction;
    RETURN_NULL_IF_FALSE(prediction != nullptr);

    m_deviceResources->ensureCameraResources(holographicFrame, prediction);
    RETURN_NULL_IF_FALSE(prediction->CameraPoses->Size != 0);

    auto pose = prediction->CameraPoses->GetAt(0);
    updateProjectionMatrices(pose);

    // Get the stationary reference frame's coordinate system
    auto stationaryCoordinateSystem = m_stationaryFrameOfReference->CoordinateSystem;
    auto viewTransformBox = pose->TryGetViewTransform(stationaryCoordinateSystem);
    if (!viewTransformBox) {
        return nullptr;
    }

    auto viewTransform = viewTransformBox->Value;
    flipYViewTransform(&viewTransform.Left, m_leftViewMatrixStoragePointer);
    flipYViewTransform(&viewTransform.Right, m_rightViewMatrixStoragePointer);

    auto predictionTimestamp = prediction->Timestamp;
    auto stationaryPointerPose = SpatialPointerPose::TryGetAtTimestamp(stationaryCoordinateSystem, predictionTimestamp);
    RETURN_NULL_IF_FALSE(stationaryPointerPose != nullptr);
    auto stationaryCsToHeadTransform = getHeadTransform(stationaryPointerPose);
    auto headToStationaryCSTransform = XMMatrixRTInverse(stationaryCsToHeadTransform);

    DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(m_orientationVectorStoragePointer),
                           XMQuaternionRotationMatrix(headToStationaryCSTransform));

    XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(m_positionVectorStoragePointer),
                  headToStationaryCSTransform.r[3]);

    return holographicFrame;
}

void MixedRealityContext::setDepthMapping(Windows::Foundation::Numerics::float4x4& projectionMatrix,
                                          double depthNear,
                                          double depthFar)
{
    // Update Z depth mapping according to VR near and far parameters
    // Update by column - translates to update row in WebGL
    projectionMatrix.m13 = 0;
    projectionMatrix.m23 = 0;

    if ((depthFar != depthNear) && depthNear != 0 && depthFar != 0) {
        projectionMatrix.m33 = static_cast<float>(-(depthFar + depthNear) / (depthFar - depthNear));
        projectionMatrix.m43 = static_cast<float>(-2 * depthFar * depthNear / (depthFar - depthNear));
    } else {
        // Avoid division by 0
        projectionMatrix.m33 = 0;
        projectionMatrix.m43 = 2 * FLT_EPSILON;
    }

    projectionMatrix.m34 = -1;
}

void MixedRealityContext::updateProjectionMatrices(Windows::Graphics::Holographic::HolographicCameraPose ^ pose)
{
    auto projection = pose->ProjectionTransform;

    // Tweak the projection matrices to use OpenGL conventinon for the depth buffer mapping.
    setDepthMapping(projection.Left, m_nearFarStoragePointer[0], m_nearFarStoragePointer[1]);
    setDepthMapping(projection.Right, m_nearFarStoragePointer[0], m_nearFarStoragePointer[1]);

    pose->HolographicCamera->SetNearPlaneDistance(m_nearFarStoragePointer[0]);
    pose->HolographicCamera->SetFarPlaneDistance(m_nearFarStoragePointer[1]);

    CopyMemory(m_leftProjectionMatrixStoragePointer, &projection.Left.m11, 16 * sizeof(float));
    CopyMemory(m_rightProjectionMatrixStoragePointer, &projection.Right.m11, 16 * sizeof(float));
}

void MixedRealityContext::flipYViewTransform(Windows::Foundation::Numerics::float4x4* viewTransform, float* destination)
{
    DirectX::XMVECTOR scale;
    DirectX::XMVECTOR position;
    DirectX::XMVECTOR orientation;

    auto viewMatrix = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(viewTransform));
    DirectX::XMMatrixDecompose(&scale, &orientation, &position, viewMatrix);

    auto negateXZ = XMVectorSet(-1.0, 1.0, -1.0, 1.0);
    auto negateY = XMVectorSet(1.0, -1.0, 1.0, 1.0);

    auto rotation = XMMatrixRotationQuaternion(orientation * negateXZ);
    auto scaleM = XMMatrixScalingFromVector(scale * negateY);
    auto positionM = XMMatrixTranslationFromVector(position * negateY);
    auto newView = scaleM * rotation * positionM;

    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(destination), newView);

    /*viewTransform.m12 = -viewTransform.m12;
    viewTransform.m22 = -viewTransform.m22;
    viewTransform.m32 = -viewTransform.m32;
    viewTransform.m42 = -viewTransform.m42;*/
}

HRESULT
MixedRealityContext::render(HolographicFrame ^ holographicFrame, ID3D11Texture2D* content)
{
    // Lock the set of holographic camera resources, then draw to each camera
    // in this frame.
    return m_deviceResources->useHolographicCameraResources<HRESULT>(
        [this, holographicFrame, content](std::map<UINT32, std::unique_ptr<CameraResources>>& cameraResourceMap) {
            auto prediction = holographicFrame->CurrentPrediction;

            for (unsigned int i = 0; i < prediction->CameraPoses->Size; i++) {
                auto cameraPose = prediction->CameraPoses->GetAt(i);

                // This represents the device-based resources for a HolographicCamera.
                CameraResources* pCameraResources = cameraResourceMap[cameraPose->HolographicCamera->Id].get();

                D3D11_TEXTURE2D_DESC contentDescription;
                content->GetDesc(&contentDescription);

                auto backbuffer = pCameraResources->GetBackBufferTexture2D();

                D3D11_BOX leftSource;
                leftSource.top = 0;
                leftSource.left = 0;
                leftSource.bottom = contentDescription.Height;
                leftSource.right = contentDescription.Width / 2;
                leftSource.front = 0;
                leftSource.back = 1;

                D3D11_BOX rightSource;
                rightSource.top = 0;
                rightSource.left = contentDescription.Width / 2;
                rightSource.bottom = contentDescription.Height;
                rightSource.right = contentDescription.Width;
                rightSource.front = 0;
                rightSource.back = 1;

                const auto context = m_deviceResources->getD3DDeviceContext();
                context->CopySubresourceRegion(backbuffer, 0, 0, 0, 0, content, 0, &leftSource);
                context->CopySubresourceRegion(backbuffer, 1, 0, 0, 0, content, 0, &rightSource);
            }

            return S_OK;
        });
}

void MixedRealityContext::present(HolographicFrame ^ holographicFrame)
{
    m_presentPerformance.end();
    m_deviceResources->present(holographicFrame);
    m_presentPerformance.start();
}

void MixedRealityContext::OnCameraAdded(HolographicSpace ^ sender, HolographicSpaceCameraAddedEventArgs ^ args)
{
    m_cameraCount++;

    m_renderTargetSize = args->Camera->RenderTargetSize;

    Deferral ^ deferral = args->GetDeferral();
    HolographicCamera ^ holographicCamera = args->Camera;
    create_task([this, deferral, holographicCamera]() {
        m_deviceResources->addHolographicCamera(holographicCamera);
        deferral->Complete();
    });
}

void MixedRealityContext::OnCameraRemoved(HolographicSpace ^ sender, HolographicSpaceCameraRemovedEventArgs ^ args)
{
    m_scriptWindow->setHeadsetAvailable(false);
    m_cameraCount--;
    m_deviceResources->removeHolographicCamera(args->Camera);
}