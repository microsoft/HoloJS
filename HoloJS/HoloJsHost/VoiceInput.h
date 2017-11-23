#pragma once

#include "ChakraForHoloJS.h"
#include "InputInterface.h"

namespace HologramJS {
namespace Input {

class VoiceInput {
   public:
    VoiceInput();
    ~VoiceInput();

    void SetScriptCallback(JsValueRef scriptCallback) { m_scriptCallback = scriptCallback; }

    bool AddEventListener(const std::wstring& type);

    bool RemoveEventListener(const std::wstring& type);

    concurrency::task<void> SetVoiceCommands(std::vector<std::wstring> voiceCommands);

   private:
    JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;

    unsigned int m_inputRefCount = 0;

    concurrency::task<void> Initialize();

    concurrency::task<void> Shutdown();

    concurrency::task<bool> CompileVoiceCommandsAsync();

    Windows::Media::SpeechRecognition::SpeechRecognizer ^ m_speechRecognizer;
    static void OnResultGenerated(
        Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^ sender,
        Windows::Media::SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^ args);

    Windows::Foundation::EventRegistrationToken m_resultGeneratedToken;

    std::vector<std::wstring> m_voiceCommands;

    std::mutex m_recognizerLock;
};
}  // namespace Input
}  // namespace HologramJS
