#pragma once

#include "holojs/private/holojs-script-host-internal.h"
#include <functional>
#include <vector>
#include <string>

namespace HoloJs {
namespace Platforms {
namespace Win32 {
class VoiceInput {
   public:
    VoiceInput(HoloJs::IHoloJsScriptHostInternal* host) : m_host(host) {}

    HRESULT start();
    HRESULT stop();
    void setOnResultsCallback(std::function<void(std::wstring command, double confidence)> callback)
    {
        m_callback = callback;
    }

    void setVoiceCommands(std::vector<std::wstring> commands) { m_voiceCommands = commands; }

   private:
    Windows::Media::SpeechRecognition::SpeechRecognizer ^ m_speechRecognizer;

    void onResultGenerated(Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^ sender,
                           Windows::Media::SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^
                               args);

    Windows::Foundation::EventRegistrationToken m_resultGeneratedToken;

    void compileVoiceCommandsAsync(std::vector<std::wstring>& commands);

    std::function<void(std::wstring command, double confidence)> m_callback;

    HoloJs::IHoloJsScriptHostInternal* m_host;

    std::vector<std::wstring> m_voiceCommands;
};

}  // namespace Win32
}  // namespace Platform
}  // namespace HoloJs
