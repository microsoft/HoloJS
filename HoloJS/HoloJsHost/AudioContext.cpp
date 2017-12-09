#include "pch.h"
#include "AudioContext.h"
#include "AudioBufferSourceNode.h"
#include "AudioDestinationNode.h"
#include "AudioNode.h"
#include "GainNode.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"
#include "SoundBuffer.h"
#include "PannerNode.h"

using namespace HologramJS::Audio;
using namespace HologramJS::Utilities;
using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Core;
using namespace std;

AudioContext::AudioContext() { m_context = lab::MakeAudioContext(); }

AudioContext::~AudioContext() { lab::CleanupAudioContext(m_context); }

bool AudioContext::InitializeProjections()
{
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createGain", L"audioContext", createGainStatic));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"getDestination", L"audioContext", getDestination));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createBufferSource", L"audioContext", createBufferSource));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"createPanner", L"audioContext", createPanner));

    return true;
}

JsValueRef CHAKRA_CALLBACK AudioContext::createPanner(JsValueRef callee,
    bool isConstructCall,
    JsValueRef* arguments,
    unsigned short argumentCount,
    PVOID callbackData)
{
    RETURN_IF_FALSE(argumentCount == 2);

    auto audioContext = ScriptResourceTracker::ExternalToObject<AudioContext>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(audioContext);

    auto newPannerNode = new PannerNode(audioContext->m_context, std::make_shared<lab::PannerNode>(audioContext->m_context->sampleRate()));
    return ScriptResourceTracker::ObjectToDirectExternal(newPannerNode);
}

JsValueRef CHAKRA_CALLBACK AudioContext::createGainStatic(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_IF_FALSE(argumentCount == 2);

    AudioContext* context = ScriptResourceTracker::ExternalToObject<AudioContext>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(context);

    return context->createGain();
}

JsValueRef CHAKRA_CALLBACK AudioContext::getDestination(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_IF_FALSE(argumentCount == 2);

    AudioContext* context = ScriptResourceTracker::ExternalToObject<AudioContext>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(context);

    return ScriptResourceTracker::ObjectToDirectExternal(
        new AudioDestinationNode(context->m_context, context->m_context->destination()));
}

JsValueRef CHAKRA_CALLBACK AudioContext::createBufferSource(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_IF_FALSE(argumentCount == 2);

    AudioContext* context = ScriptResourceTracker::ExternalToObject<AudioContext>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(context);

    auto audioBufferSourceNode = make_shared<lab::AudioBufferSourceNode>(context->m_context->sampleRate());

    return ScriptResourceTracker::ObjectToDirectExternal(
        new AudioBufferSourceNode(context->m_context, audioBufferSourceNode));
}

JsValueRef AudioContext::createGain()
{
    auto newGainNode = new GainNode(m_context, std::make_shared<lab::GainNode>(m_context->sampleRate()));
    m_audioNodes.push_back(newGainNode);
    return ScriptResourceTracker::ObjectToDirectExternal(newGainNode);
}

bool AudioContext::DecodeAudioData(JsValueRef data, JsValueRef onSuccess, JsValueRef onError)
{
    JsValueType dataType;
    RETURN_IF_JS_ERROR(JsGetValueType(data, &dataType));

    if (dataType != JsArrayBuffer) {
        return false;
    }

    if (onSuccess == JS_INVALID_REFERENCE) {
        return false;
    }

    RETURN_IF_JS_ERROR(JsAddRef(onSuccess, nullptr));

    byte* buffer;
    unsigned int bufferLength;
    RETURN_IF_JS_ERROR(JsGetArrayBufferStorage(data, &buffer, &bufferLength));

    vector<uint8_t> capturedBuffer(bufferLength);
    memcpy(capturedBuffer.data(), buffer, bufferLength);
    shared_ptr<lab::SoundBuffer> soundBuffer =
        make_shared<lab::SoundBuffer>(capturedBuffer, "ogg", m_context->sampleRate());

    /*std::shared_ptr<lab::GainNode> gain;
    std::shared_ptr<lab::AudioBufferSourceNode> tonbiSound;

    

    auto lockedArea = [&](lab::ContextGraphLock& g, lab::ContextRenderLock& r) {
        gain = std::make_shared<lab::GainNode>(m_context->sampleRate());
        gain->gain()->setValue(0.0625f);

        // gain -> destination
        auto audioNode = static_cast<lab::AudioNode*>(gain.get());
        audioNode->connect(m_context.get(), m_context->destination().get(), 0, 0);
        tonbiSound = tonbi->play(r, 0.0f);
    };

    lab::AcquireLocksForContext("Tone and Sample App", m_context, lockedArea);

    const int seconds = 3;
    for (int t = 0; t < seconds; ++t) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }*/

    CoreApplication::MainView->CoreWindow->Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal, ref new DispatchedHandler([this, soundBuffer, onSuccess] {
            callbackScriptOnDecodeSuccess(soundBuffer, onSuccess);
        }));

    return true;
}

void AudioContext::callbackScriptOnDecodeSuccess(shared_ptr<lab::SoundBuffer> soundBuffer, JsValueRef callback)
{
    EXIT_IF_TRUE(callback == JS_INVALID_REFERENCE);

    JsValueRef parameters[2];
    parameters[0] = callback;
    parameters[1] = ScriptResourceTracker::ObjectToDirectExternal(new SoundBuffer(soundBuffer));

    JsValueRef result;
    HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(callback, parameters, ARRAYSIZE(parameters), &result));

    JsRelease(callback, nullptr);
}
