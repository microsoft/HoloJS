#pragma once
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

			Windows::Foundation::TypedEventHandler<Windows::UI::Input::Spatial::SpatialInteractionManager ^, Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs ^>^ m_eventHandler;

			void CallbackScriptForSpatialInput(Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs^ args);

			JsValueRef m_spatialInputEventName;
			JsValueRef m_spatialInputSourcePressedName;
			JsValueRef m_spatialInputSourceReleasedName;

			Windows::Perception::Spatial::SpatialStationaryFrameOfReference^ m_frameOfReference;
		};
	}
}

