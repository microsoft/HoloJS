#include "pch.h"
#include "VoiceInput.h"

using namespace HologramJS::Input;
using namespace std;
using namespace Windows::Media::SpeechRecognition;
using namespace Windows::Foundation;
using namespace Platform;
using namespace concurrency;

PCWSTR g_supporteVoiceInputEvents[] = {L"voicecommand"};

VoiceInput::VoiceInput() {}

VoiceInput::~VoiceInput() {}

task<void> VoiceInput::SetVoiceCommands(vector<wstring> voiceCommands)
{
    lock_guard<mutex> guard(m_recognizerLock);

    m_voiceCommands = voiceCommands;

    if (m_speechRecognizer == nullptr) {
        return;
    }

    if (m_speechRecognizer->State != SpeechRecognizerState::Idle) {
        await m_speechRecognizer->ContinuousRecognitionSession->StopAsync();
    }

    auto compilationResult = await CompileVoiceCommandsAsync();

    if (compilationResult) {
        await m_speechRecognizer->ContinuousRecognitionSession->StartAsync();
    }

    return;
}

task<bool> VoiceInput::CompileVoiceCommandsAsync()
{
    Platform::Collections::Vector<String ^> ^ speechCommandList = ref new Platform::Collections::Vector<String ^>();
    for (int i = 0; i < m_voiceCommands.size(); i++) {
        speechCommandList->Append(Platform::StringReference(m_voiceCommands[i].c_str()));
    }

    SpeechRecognitionListConstraint ^ spConstraint = ref new SpeechRecognitionListConstraint(speechCommandList);
    m_speechRecognizer->Constraints->Clear();
    m_speechRecognizer->Constraints->Append(spConstraint);

    auto result = await m_speechRecognizer->CompileConstraintsAsync();

    return (result->Status == SpeechRecognitionResultStatus::Success);
}

task<void> VoiceInput::Initialize()
{
    lock_guard<mutex> guard(m_recognizerLock);

    if (m_speechRecognizer != nullptr) {
        return;
    }

    m_speechRecognizer = ref new SpeechRecognizer();

    m_resultGeneratedToken = m_speechRecognizer->ContinuousRecognitionSession->ResultGenerated +=
        ref new TypedEventHandler<SpeechContinuousRecognitionSession ^,
                                  SpeechContinuousRecognitionResultGeneratedEventArgs ^>(
            &VoiceInput::OnResultGenerated);

    if (await CompileVoiceCommandsAsync()) {
        await m_speechRecognizer->ContinuousRecognitionSession->StartAsync();
    }
}

task<void> VoiceInput::Shutdown()
{
    lock_guard<mutex> guard(m_recognizerLock);

    if (m_speechRecognizer != nullptr) {
        m_speechRecognizer->ContinuousRecognitionSession->ResultGenerated -= m_resultGeneratedToken;
        if (m_speechRecognizer->State != SpeechRecognizerState::Idle) {
            await m_speechRecognizer->ContinuousRecognitionSession->StopAsync();
        }
        m_speechRecognizer = nullptr;
    }
}

bool VoiceInput::AddEventListener(const wstring& type)
{
    for (int i = 0; i < ARRAYSIZE(g_supporteVoiceInputEvents); i++) {
        if (type == g_supporteVoiceInputEvents[i]) {
            m_inputRefCount++;
            if (m_inputRefCount == 1) {
                Initialize().get();
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
            m_inputRefCount--;
            if (m_inputRefCount == 0) {
                Shutdown().get();
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
