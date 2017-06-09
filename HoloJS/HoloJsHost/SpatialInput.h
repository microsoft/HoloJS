#pragma once
#include "InputInterface.h"

namespace HologramJS
{
	namespace Input
	{
		class SpatialInput
		{
		public:
			SpatialInput();
			~SpatialInput();

			void SetScriptCallback(JsValueRef scriptCallback) { m_scriptCallback = scriptCallback; }

			void SetStationaryFrameOfReference(Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ frameOfReference)
			{
				m_frameOfReference = frameOfReference;
			}

		private:
			JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;

			Windows::Foundation::EventRegistrationToken m_sourcePressedToken;
			Windows::Foundation::EventRegistrationToken m_sourceReleasedToken;
			Windows::Foundation::EventRegistrationToken m_sourceLostToken;
			Windows::Foundation::EventRegistrationToken m_sourceDetectedToken;
			Windows::Foundation::EventRegistrationToken m_sourceUpdateToken;

			bool m_spatialInputAvailable = false;

			void CallbackScriptForSpatialInput(SpatialInputEventType type, Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs^ args);

			JsValueRef m_spatialInputEventName;
			JsValueRef m_spatialInputSourcePressedName;
			JsValueRef m_spatialInputSourceReleasedName;

			Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ m_frameOfReference;
		};
	}
}

