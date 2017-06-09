#include "pch.h"
#include "SpatialInput.h"

using namespace HologramJS::Input;
using namespace Windows::UI::Input::Spatial;
using namespace Windows::Foundation;
using namespace std;
using namespace Windows::Perception::Spatial;

SpatialInput::SpatialInput()
{
	if (!SpatialInteractionManager::GetForCurrentView())
	{
		return;
	}

	m_sourcePressedToken = SpatialInteractionManager::GetForCurrentView()->SourcePressed += ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(
		[this](SpatialInteractionManager ^sender, SpatialInteractionSourceEventArgs ^args)
	{
		CallbackScriptForSpatialInput(SpatialInputEventType::Pressed, args);
	});

	m_sourceReleasedToken = SpatialInteractionManager::GetForCurrentView()->SourceReleased += ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(
		[this](SpatialInteractionManager ^sender, SpatialInteractionSourceEventArgs ^args)
	{
		CallbackScriptForSpatialInput(SpatialInputEventType::Released, args);
	});

	m_sourceDetectedToken = SpatialInteractionManager::GetForCurrentView()->SourceDetected += ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(
		[this](SpatialInteractionManager ^sender, SpatialInteractionSourceEventArgs ^args)
	{
		CallbackScriptForSpatialInput(SpatialInputEventType::Detected, args);
	});

	m_sourceLostToken = SpatialInteractionManager::GetForCurrentView()->SourceLost += ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(
		[this](SpatialInteractionManager ^sender, SpatialInteractionSourceEventArgs ^args)
	{
		CallbackScriptForSpatialInput(SpatialInputEventType::Lost, args);
	});

	m_sourceUpdateToken = SpatialInteractionManager::GetForCurrentView()->SourceUpdated += ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(
		[this](SpatialInteractionManager ^sender, SpatialInteractionSourceEventArgs ^args)
	{
		CallbackScriptForSpatialInput(SpatialInputEventType::Update, args);
	});

	m_spatialInputAvailable = true;
}

SpatialInput::~SpatialInput()
{
	SpatialInteractionManager::GetForCurrentView()->SourceReleased -= m_sourceReleasedToken;
	SpatialInteractionManager::GetForCurrentView()->SourcePressed -= m_sourcePressedToken;
	SpatialInteractionManager::GetForCurrentView()->SourceDetected -= m_sourceDetectedToken;
	SpatialInteractionManager::GetForCurrentView()->SourceLost -= m_sourceLostToken;
	SpatialInteractionManager::GetForCurrentView()->SourceUpdated -= m_sourceUpdateToken;

}

void
SpatialInput::CallbackScriptForSpatialInput(SpatialInputEventType type, Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs^ args)
{
	EXIT_IF_TRUE(m_scriptCallback == JS_INVALID_REFERENCE);
	EXIT_IF_FALSE(m_spatialInputAvailable);

	JsValueRef parameters[8];
	parameters[0] = m_scriptCallback;
	JsValueRef* eventTypeParam = &parameters[1];
	JsValueRef* xParam = &parameters[2];
	JsValueRef* yParam = &parameters[3];
	JsValueRef* zParam = &parameters[4];
	JsValueRef* isPressedParam = &parameters[5];
	JsValueRef* sourceKindParam = &parameters[6];
	JsValueRef* spatialInputTypeParam = &parameters[7];

	const auto location = args->State->Properties->TryGetLocation(m_frameOfReference->CoordinateSystem);
	const float x = location ? location->Position->Value.x : 0;
	const float y = location ? location->Position->Value.y : 0;
	const float z = location ? location->Position->Value.z : 0;

	EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(NativeToScriptInputType::SpatialInput), eventTypeParam));
	EXIT_IF_JS_ERROR(JsDoubleToNumber(x, xParam));
	EXIT_IF_JS_ERROR(JsDoubleToNumber(y, yParam));
	EXIT_IF_JS_ERROR(JsDoubleToNumber(z, zParam));
	EXIT_IF_JS_ERROR(JsBoolToBoolean(args->State->IsPressed, isPressedParam));
	EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(args->State->Source->Kind), sourceKindParam));
	EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(type), spatialInputTypeParam));

	JsValueRef result;
	EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
}
