#include "pch.h"
#include "VoiceInput.h"

using namespace HologramJS::Input;
using namespace std;
using namespace Windows::Media::SpeechRecognition;
using namespace Windows::Foundation;
using namespace Platform;
using namespace concurrency;
using namespace Windows::UI::Core;

PCWSTR g_supporteVoiceInputEvents[] = {L"voicecommand"};

VoiceInput::VoiceInput()
{
    m_speechRecognizer = ref new SpeechRecognizer();

    m_resultGeneratedToken = m_speechRecognizer->ContinuousRecognitionSession->ResultGenerated +=
        ref new TypedEventHandler<SpeechContinuousRecognitionSession ^,
                                  SpeechContinuousRecognitionResultGeneratedEventArgs ^>(
            &VoiceInput::OnResultGenerated);
}

VoiceInput::~VoiceInput() {}

void VoiceInput::SetVoiceCommands(vector<wstring> voiceCommands)
{
    m_voiceCommands = voiceCommands;

    if (m_isRunning) {
        m_eventsQueue.push(EventType::Set);
        create_task([this]() { ProcessQueueEntry(); });
    }
}

task<void> VoiceInput::ResetVoiceCommandsAsync()
{
    await m_speechRecognizer->ContinuousRecognitionSession->CancelAsync();

    await StartAsync();
}

task<SpeechRecognitionCompilationResult ^> VoiceInput::CompileVoiceCommandsAsync()
{
    Platform::Collections::Vector<String ^> ^ speechCommandList = ref new Platform::Collections::Vector<String ^>();
    for (int i = 0; i < m_voiceCommands.size(); i++) {
        speechCommandList->Append(Platform::StringReference(m_voiceCommands[i].c_str()));
    }

    SpeechRecognitionListConstraint ^ spConstraint = ref new SpeechRecognitionListConstraint(speechCommandList);
    m_speechRecognizer->Constraints->Clear();
    m_speechRecognizer->Constraints->Append(spConstraint);

    std::shared_ptr<concurrency::event> _completed = std::make_shared<concurrency::event>();

    return create_task(m_speechRecognizer->CompileConstraintsAsync());
}

// Process speech recognize requests: start, stop, change commands
// This method runs synchronously on a background thread and
// sequentializes the incoming asynchronous speech recognizer requests
void VoiceInput::ProcessQueueEntry()
{
    EventType queuedEvent;
    if (!m_eventsQueue.try_pop(queuedEvent)) {
        return;
    }

    m_recognizerLock.lock();

    if (queuedEvent == EventType::Start) {
        if (!m_isRunning) {
            StartAsync().wait();
            m_isRunning = true;
        }
    } else if (queuedEvent == EventType::Set) {
        if (m_isRunning) {
            ResetVoiceCommandsAsync().wait();
        }
    } else if (queuedEvent == EventType::Stop) {
        if (m_isRunning) {
            create_task(m_speechRecognizer->ContinuousRecognitionSession->CancelAsync()).wait();
            m_isRunning = false;
        }
    }

    m_recognizerLock.unlock();
}

task<void> VoiceInput::StartAsync()
{
    auto result = await CompileVoiceCommandsAsync();
    if (result->Status == SpeechRecognitionResultStatus::Success) {
        await m_speechRecognizer->ContinuousRecognitionSession->StartAsync();
    }
}

bool VoiceInput::AddEventListener(const wstring& type)
{
    for (int i = 0; i < ARRAYSIZE(g_supporteVoiceInputEvents); i++) {
        if (type == g_supporteVoiceInputEvents[i]) {
			// AddEventListener and RemoveEventListener are always called on the UI thread; as such
			// there is no need to synchronize access to the input ref count
            m_inputRefCount++;
            if (m_inputRefCount == 1) {
                m_eventsQueue.push(EventType::Start);
                create_task([this]() { ProcessQueueEntry(); });
            }

            return true;
        }
    }
    return false;
}

bool VoiceInput::RemoveEventListener(const wstring& type)
{
    for (int i = 0; i < ARRAYSIZE(g_supporteVoiceInputEvents); i++) {
        if (type == g_supporteVoiceInputEvents[i]) {
			// AddEventListener and RemoveEventListener are always called on the UI thread; as such
			// there is no need to synchronize access to the input ref count
            if (m_inputRefCount == 1) {
				m_inputRefCount = 0;
                m_eventsQueue.push(EventType::Stop);
                create_task([this]() { ProcessQueueEntry(); });
            } else if (m_inputRefCount > 1) {
				m_inputRefCount--;
            }

            return true;
        }
    }
    return false;
}

void VoiceInput::OnResultGenerated(SpeechContinuousRecognitionSession ^ sender,
                                   SpeechContinuousRecognitionResultGeneratedEventArgs ^ args)
{
}
