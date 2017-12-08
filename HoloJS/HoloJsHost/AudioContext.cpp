#include "pch.h"
#include "AudioContext.h"

using namespace HologramJS::Audio;
using namespace std;

#include "LabSound/extended/LabSound.h"

AudioContext::AudioContext() {}

AudioContext::~AudioContext()
{
    if (m_audioContentRef != JS_INVALID_REFERENCE) {
        JsRelease(m_audioContentRef, nullptr);
        m_audioContentRef = JS_INVALID_REFERENCE;
    }
}

bool AudioContext::DecodeAudioData(JsValueRef data, JsValueRef onSuccess, JsValueRef onError)
{
    JsValueType dataType;
    RETURN_IF_JS_ERROR(JsGetValueType(data, &dataType));

    if (dataType != JsArrayBuffer) {
        return false;
    }

    RETURN_IF_JS_ERROR(JsAddRef(data, nullptr));
    m_audioContentRef = data;

    byte* buffer;
    unsigned int bufferLength;
    RETURN_IF_JS_ERROR(JsGetArrayBufferStorage(data, &buffer, &bufferLength));

	auto context = lab::MakeAudioContext();

	auto ac = context.get();

	std::shared_ptr<lab::OscillatorNode> oscillator;
	lab::SoundBuffer tonbi("scripts\\sample.ogg", context->sampleRate());
	std::shared_ptr<lab::GainNode> gain;
	std::shared_ptr<lab::AudioBufferSourceNode> tonbiSound;

	auto lockedArea = [&](lab::ContextGraphLock & g, lab::ContextRenderLock & r)
	{
		oscillator = std::make_shared<lab::OscillatorNode>(r, context->sampleRate());
		gain = std::make_shared<lab::GainNode>(context->sampleRate());
		gain->gain()->setValue(0.0625f);

		// osc -> gain -> destination
		oscillator->connect(ac, gain.get(), 0, 0);
		gain->connect(ac, context->destination().get(), 0, 0);
		oscillator->start(0);
		oscillator->frequency()->setValue(440.f);
		oscillator->setType(r, lab::OscillatorType::SINE);
		tonbiSound = tonbi.play(r, 0.0f);
	};

	lab::AcquireLocksForContext("Tone and Sample App", context, lockedArea);

	const int seconds = 12;
	for (int t = 0; t < seconds; ++t)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	lab::CleanupAudioContext(context);

	return true;
}
