#include "stdafx.h"
#include "../host-interfaces.h"
#include "win32-spatial-input.h"
#include "holojs/private/script-host-utilities.h"
#include <..\um\SpatialInteractionManagerInterop.h>
#include <..\winrt\WinRTBase.h>
#include <Windows.UI.Input.Spatial.h>
#include <ppltasks.h>

using namespace Windows::UI::Input::Spatial;
using namespace std::placeholders;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace Windows::Foundation;
using namespace HoloJs::Interfaces;
using namespace std;
using namespace HoloJs::Win32;

HRESULT Win32SpatialInput::createSpatialInteractionManager()
{
    ComPtr<ABI::Windows::UI::Input::Spatial::ISpatialInteractionManagerStatics> spSpatialInteractionManagerStatics;

    RETURN_IF_FAILED(ABI::Windows::Foundation::GetActivationFactory(
        HStringReference(RuntimeClass_Windows_UI_Input_Spatial_SpatialInteractionManager).Get(),
        &spSpatialInteractionManagerStatics));

    ComPtr<ISpatialInteractionManagerInterop> spSpatialInteractionManagerInterop;
    RETURN_IF_FAILED(spSpatialInteractionManagerStatics.As(&spSpatialInteractionManagerInterop));

    ComPtr<ABI::Windows::UI::Input::Spatial::ISpatialInteractionManager> spatialInteractionManagerWin32;
    RETURN_IF_FAILED(spSpatialInteractionManagerInterop->GetForWindow(
        m_window,
        __uuidof(ABI::Windows::UI::Input::Spatial::ISpatialInteractionManager),
        &spatialInteractionManagerWin32));

    m_spatialInteractionManager = safe_cast<Windows::UI::Input::Spatial::ISpatialInteractionManager ^>(
        reinterpret_cast<Platform::Object ^>(spatialInteractionManagerWin32.Get()));

    return S_OK;
}

