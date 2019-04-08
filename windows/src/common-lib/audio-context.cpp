#include "stdafx.h"
#include "audio-buffer-source.h"
#include "audio-buffer.h"
#include "audio-context.h"
#include "audio-destination-node.h"
#include "audio-listener.h"
#include "audio-panner-node.h"
#include "gain-node.h"
#include "holojs/holojs-script-host.h"
#include "holojs/private/app-model.h"
#include "holojs/private/error-handling.h"
#include "holojs/private/object-lifetime.h"
#include "holojs/private/platform-interfaces.h"
#include "holojs/private/script-host-utilities.h"

using namespace HoloJs::Win32::WebAudio;
using namespace HoloJs;
using namespace std;

AudioContext::~AudioContext() {}

IGainNode* AudioContext::createGain()
{
    auto newGainNode = new GainNode(m_context, make_shared<lab::GainNode>());
    return newGainNode;
}

IAudioDestinationNode* AudioContext::getDestination()
{
    return new AudioDestinationNode(m_context, m_context->destination());
};

IPannerNode* AudioContext::createPanner()
{
    return new PannerNode(m_context, std::make_shared<lab::PannerNode>(m_context->sampleRate()));
}

IAudioListener* AudioContext::getListener() { return new AudioListener(m_context); }

long AudioContext::decodeAudioData(JsValueRef data, JsValueRef onSuccessCallback, JsValueRef onFailedCallback)
{
    JsValueType dataType;
    RETURN_IF_JS_ERROR(JsGetValueType(data, &dataType));
    RETURN_IF_TRUE(dataType != JsArrayBuffer);

    if (onFailedCallback != JS_INVALID_REFERENCE) {
        RETURN_IF_JS_ERROR(JsAddRef(onFailedCallback, nullptr));
    }

    RETURN_IF_JS_ERROR(JsAddRef(onSuccessCallback, nullptr));
    RETURN_IF_JS_ERROR(JsAddRef(data, nullptr));

    byte* buffer;
    unsigned int bufferLength;
    RETURN_IF_JS_ERROR(JsGetArrayBufferStorage(data, &buffer, &bufferLength));

    m_host->runInBackground([this, buffer, bufferLength, data, onSuccessCallback, onFailedCallback]() -> long {
        std::shared_ptr<lab::AudioBus> audioFile;
        // WAV magic: RIFF****WAVE
        PCSTR wavMagic1 = "RIFF";
        PCSTR wavMagic2 = "WAVE";
        PCSTR oggMagic = "OggS";
        PCSTR mp3Magic = "ID3";

        PCSTR soundExtension = nullptr;

        if (memcmp(buffer, wavMagic1, strlen(wavMagic1)) == 0 &&
            memcmp(buffer + strlen(wavMagic1) + 4, wavMagic2, strlen(wavMagic2)) == 0) {
            soundExtension = "wav";
        } else if (memcmp(buffer, oggMagic, strlen(oggMagic)) == 0) {
            soundExtension = "ogg";
        } else if ((memcmp(buffer, mp3Magic, strlen(mp3Magic)) == 0) || (buffer[0] == 0xff && buffer[1] == 0xfb)) {
            soundExtension = "mp3";
        }

        if (soundExtension != nullptr) {
            vector<uint8_t> capturedBuffer(bufferLength);
            memcpy(capturedBuffer.data(), buffer, bufferLength);

            lab::ContextRenderLock r(m_context.get(), "TestAudioFile");
            audioFile = lab::MakeBusFromMemory(capturedBuffer, soundExtension, false);
        }

        m_host->runInScriptContext(
            [this, audioFile, data, onSuccessCallback, onFailedCallback]() -> void {
                JsValueRef result;

                if (audioFile) {
                    JsValueRef audioBufferRef;
                    unique_ptr<AudioBuffer> audioFileTemp;
                    audioFileTemp.reset(new AudioBuffer(audioFile));

                    EXIT_IF_FAILED(m_host->createScriptObject(
                        audioFileTemp.get(), HoloJs::ResourceManagement::ObjectType::IAudioBuffer, &audioBufferRef));

                    // The native pointer is not tracked by the audioBufferRef;
                    audioFileTemp.release();

                    JsValueRef args[2] = {onSuccessCallback, audioBufferRef};

                    HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(onSuccessCallback, args, ARRAYSIZE(args), &result));
                } else if (onFailedCallback != JS_INVALID_REFERENCE) {
                    JsValueRef args[1] = {onFailedCallback};
                    HANDLE_EXCEPTION_IF_JS_ERROR(JsCallFunction(onFailedCallback, args, ARRAYSIZE(args), &result));
                }

                EXIT_IF_JS_ERROR(JsRelease(data, nullptr));
                EXIT_IF_JS_ERROR(JsRelease(onSuccessCallback, nullptr));
                if (onFailedCallback != JS_INVALID_REFERENCE) {
                    EXIT_IF_JS_ERROR(JsRelease(onFailedCallback, nullptr));
                }
            });

        return S_OK;
    });

    return S_OK;
}

IAudioBufferSource* AudioContext::createAudioBufferSource()
{
    auto labAudioBufferSourceNode = make_shared<lab::SampledAudioNode>();
    auto audioBufferSource = new AudioBufferSource(m_context, labAudioBufferSourceNode);
    auto audioBufferSourceCallbackContext = audioBufferSource->m_isValid;

    labAudioBufferSourceNode->setOnEnded([this, audioBufferSource, audioBufferSourceCallbackContext]() {
        m_host->runInScriptContext(
            [audioBufferSource, audioBufferSourceCallbackContext]() -> void {
                if (*audioBufferSourceCallbackContext) {
                    audioBufferSource->invokeEventListeners(L"ended");
                }
            });
    });

    return audioBufferSource;
}