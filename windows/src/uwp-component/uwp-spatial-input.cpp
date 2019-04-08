#include "stdafx.h"
#include "uwp-spatial-input.h"

using namespace Windows::UI::Input::Spatial;
using namespace std;
using namespace HoloJs::UWP;

HRESULT UWPSpatialInput::createSpatialInteractionManager()
{
    m_spatialInteractionManager = SpatialInteractionManager::GetForCurrentView();

    return S_OK;
}

