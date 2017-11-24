#pragma once

#include "ChakraForHoloJS.h"
#include "InputInterface.h"
#include <concrt.h>
#include <concurrent_queue.h>
#include <queue>

namespace HologramJS {
namespace Input {

class VoiceInput {
   public:
    VoiceInput();
    ~VoiceInput();

    void SetScriptCallback(JsValueRef scriptCallback) { m_scriptCallback = scriptCallback; }

    bool AddEventListener(const std::wstring& type);

    bool RemoveEventListener(const std::wstring& type);

    void SetVoiceCommands(std::vector<std::wstring> voiceCommands);

   private:
    JsValueRef m_scriptCallback = JS_INVALID_REFERENCE;

    unsigned int m_inputRefCount = 0;

    concurrency::task<void> StartAsync();

    concurrency::task<void> ResetVoiceCommandsAsync();

    concurrency::task<Windows::Media::SpeechRecognition::SpeechRecognitionCompilationResult ^>
    CompileVoiceCommandsAsync();

    Windows::Media::SpeechRecognition::SpeechRecognizer ^ m_speechRecognizer;
    void OnResultGenerated(Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^ sender,
                           Windows::Media::SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^
                               args);

    Windows::Foundation::EventRegistrationToken m_resultGeneratedToken;

    std::vector<std::wstring> m_voiceCommands;

    void ProcessQueueEntry();

    enum class EventType { Set, Start, Stop };

    concurrency::concurrent_queue<EventType> m_eventsQueue;

    std::mutex m_recognizerLock;
    bool m_isRunning = false;

    void CallbackScriptForVoiceInput(
        Windows::Media::SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^ args);
};
}  // namespace Input
}  // namespace HologramJS
