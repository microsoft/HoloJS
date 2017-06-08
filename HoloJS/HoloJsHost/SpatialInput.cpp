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

	m_eventHandler = ref new TypedEventHandler<SpatialInteractionManager ^, SpatialInteractionSourceEventArgs ^>(
		[this](SpatialInteractionManager ^sender, SpatialInteractionSourceEventArgs ^args)
	{
		this->CallbackScriptForSpatialInput(args);
	});

	m_sourcePressedToken = SpatialInteractionManager::GetForCurrentView()->SourcePressed += m_eventHandler;
	m_sourceReleasedToken = SpatialInteractionManager::GetForCurrentView()->SourceReleased += m_eventHandler;
	m_sourceDetectedToken = SpatialInteractionManager::GetForCurrentView()->SourceDetected += m_eventHandler;
	m_sourceLostToken = SpatialInteractionManager::GetForCurrentView()->SourceLost += m_eventHandler;
	m_sourceUpdateToken = SpatialInteractionManager::GetForCurrentView()->SourceUpdated += m_eventHandler;

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
SpatialInput::CallbackScriptForSpatialInput(Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs^ args)
{
	static std::wstring eventName(L"spatialinput");

	EXIT_IF_TRUE(m_scriptCallback == JS_INVALID_REFERENCE);
	EXIT_IF_FALSE(m_spatialInputAvailable);

	JsValueRef parameters[7];
	parameters[0] = m_scriptCallback;
	JsValueRef* eventNameParam = &parameters[1];
	JsValueRef* xParam = &parameters[2];
	JsValueRef* yParam = &parameters[3];
	JsValueRef* zParam = &parameters[4];
	JsValueRef* isPressedParam = &parameters[5];
	JsValueRef* kindParam = &parameters[6];

	const auto location = args->State->Properties->TryGetLocation(m_frameOfReference->CoordinateSystem);
	const float x = location ? location->Position->Value.x : 0;
	const float y = location ? location->Position->Value.y : 0;
	const float z = location ? location->Position->Value.z : 0;

	EXIT_IF_JS_ERROR(JsPointerToString(eventName.c_str(), eventName.length(), eventNameParam));
	EXIT_IF_JS_ERROR(JsDoubleToNumber(x, xParam));
	EXIT_IF_JS_ERROR(JsDoubleToNumber(y, yParam));
	EXIT_IF_JS_ERROR(JsDoubleToNumber(z, zParam));
	EXIT_IF_JS_ERROR(JsBoolToBoolean(args->State->IsPressed, isPressedParam));
	EXIT_IF_JS_ERROR(JsIntToNumber(static_cast<int>(args->State->Source->Kind), kindParam));

	JsValueRef result;
	EXIT_IF_JS_ERROR(JsCallFunction(m_scriptCallback, parameters, ARRAYSIZE(parameters), &result));
}
