#include "stdafx.h"
#include "../host-interfaces.h"
#include "holojs/private/script-host-utilities.h"
#include "include/holojs/windows/mixed-reality/spatial-input.h"
#include <ppltasks.h>

using namespace Windows::UI::Input::Spatial;
using namespace std::placeholders;
using namespace Windows::Foundation;
using namespace HoloJs::Interfaces;
using namespace std;
using namespace HoloJs::MixedReality::Input;

SpatialInput::~SpatialInput()
{
    if (m_spatialInteractionManager) {
        m_spatialInteractionManager->SourceDetected -= m_sourceDetectedToken;
        m_spatialInteractionManager->SourceLost -= m_sourceLostToken;
        m_spatialInteractionManager->SourceUpdated -= m_sourceUpdatedToken;

		m_spatialInteractionManager->SourcePressed -= m_sourcePressed;
		m_spatialInteractionManager->SourceReleased -= m_sourceReleased;
    }
}

HRESULT SpatialInput::initialize()
{
    RETURN_IF_FAILED(createSpatialInteractionManager());
    setupEventHandlers();

    return S_OK;
}

void SpatialInput::setupEventHandlers()
{
    m_sourcePressed = m_spatialInteractionManager->SourcePressed += ref new Windows::Foundation::TypedEventHandler<
         SpatialInteractionManager ^
         , Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs ^>(
        std::bind(&SpatialInput::OnSourceUpdated, this, _1, _2));

    m_sourceReleased = m_spatialInteractionManager->SourceReleased += ref new Windows::Foundation::TypedEventHandler<
         SpatialInteractionManager ^
         , Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs ^>(
        std::bind(&SpatialInput::OnSourceUpdated, this, _1, _2));

    m_sourceDetectedToken = m_spatialInteractionManager->SourceDetected +=
        ref new Windows::Foundation::TypedEventHandler<
            SpatialInteractionManager ^
            , Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs ^>(
            std::bind(&SpatialInput::OnSourceDetected, this, _1, _2));

    m_sourceLostToken = m_spatialInteractionManager->SourceLost +=
        ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(
            std::bind(&SpatialInput::OnSourceLost, this, _1, _2));

    m_sourceUpdatedToken = m_spatialInteractionManager->SourceUpdated +=
        ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(
            std::bind(&SpatialInput::OnSourceUpdated, this, _1, _2));
}

void SpatialInput::OnSourceDetected(SpatialInteractionManager ^ sender, SpatialInteractionSourceEventArgs ^ args)
{
    SpatialInteractionSourceState ^ state = args->State;
    SpatialInteractionSource ^ source = state->Source;

    if ((source->Kind == SpatialInteractionSourceKind::Hand) ||
        ((source->Kind == SpatialInteractionSourceKind::Controller) && source->IsPointingSupported)) {
        auto it = m_controllersMap.find(source->Id);
        if (it == m_controllersMap.end()) {
            auto spatialController = make_shared<SpatialController>(source);
            EXIT_IF_FAILED(spatialController->projectToScript());
            m_scriptWindow->gamepadConnected(spatialController->getScriptControllerObject());
            m_controllersMap.emplace(source->Id, spatialController);
        }
    }
}

void SpatialInput::OnSourceLost(SpatialInteractionManager ^ sender, SpatialInteractionSourceEventArgs ^ args)
{
    SpatialInteractionSourceState ^ state = args->State;
    SpatialInteractionSource ^ source = state->Source;

    if ((source->Kind == SpatialInteractionSourceKind::Hand) ||
        ((source->Kind == SpatialInteractionSourceKind::Controller) && source->IsPointingSupported)) {
        auto it = m_controllersMap.find(source->Id);
        if (it != m_controllersMap.end()) {
            m_scriptWindow->gamepadDisconnected(it->second->getScriptControllerObject());
            m_controllersMap.erase(source->Id);
        }
    }
}

void SpatialInput::OnSourceUpdated(SpatialInteractionManager ^ sender, SpatialInteractionSourceEventArgs ^ args)
{
    SpatialInteractionSource ^ source = args->State->Source;

    auto it = m_controllersMap.find(source->Id);
    if (it != m_controllersMap.end()) {
        auto location = args->State->Properties->TryGetLocation(m_stationaryFrameOfReference->CoordinateSystem);
        if (location) {
            it->second->update(args->State, location);
        }
    }
}
