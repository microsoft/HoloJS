#include "stdafx.h"
#include "include/holojs/windows/voice-input.h"
#include <collection.h>
#include <ppltasks.h>

using namespace concurrency;
using namespace HoloJs::Platforms::Win32;
using namespace Windows::Media::SpeechRecognition;
using namespace Windows::Foundation;
using namespace std;

HRESULT VoiceInput::start(vector<wstring> commands)
{
    m_speechRecognizer = ref new SpeechRecognizer();

    m_resultGeneratedToken = m_speechRecognizer->ContinuousRecognitionSession->ResultGenerated +=
        ref new TypedEventHandler<SpeechContinuousRecognitionSession ^,
                                  SpeechContinuousRecognitionResultGeneratedEventArgs ^>(
            [this](SpeechContinuousRecognitionSession ^ session,
                   SpeechContinuousRecognitionResultGeneratedEventArgs ^ args) { onResultGenerated(session, args); });

    create_task([this, commands]() {
        Platform::Collections::Vector<Platform::String ^> ^ speechCommandList =
            ref new Platform::Collections::Vector<Platform::String ^>();
        for (size_t i = 0; i < commands.size(); i++) {
            speechCommandList->Append(Platform::StringReference(commands[i].c_str()));
        }

        SpeechRecognitionListConstraint ^ spConstraint = ref new SpeechRecognitionListConstraint(speechCommandList);
        m_speechRecognizer->Constraints->Clear();
        m_speechRecognizer->Constraints->Append(spConstraint);

        auto compileAsync = m_speechRecognizer->CompileConstraintsAsync();
        create_task(compileAsync).then([this](SpeechRecognitionCompilationResult ^ result) {
            if (result->Status == SpeechRecognitionResultStatus::Success) {
                auto startAsync = m_speechRecognizer->ContinuousRecognitionSession->StartAsync();
                auto startTask = create_task(startAsync);
                startTask.then([this](task<void> startTask) {
                    try {
                        startTask.get();
                    } catch (...) {
                    }
                });
            }
        });
    });

    return S_OK;
}

HRESULT VoiceInput::stop() { return S_OK; }

void VoiceInput::onResultGenerated(SpeechContinuousRecognitionSession ^ sender,
                                   SpeechContinuousRecognitionResultGeneratedEventArgs ^ args)
{
    wstring command = args->Result->Text->Data();
    double confidence = args->Result->RawConfidence;
    m_callback(command, confidence);
}

void VoiceInput::compileVoiceCommandsAsync(vector<wstring>& commands)
{
    Platform::Collections::Vector<Platform::String ^> ^ speechCommandList =
        ref new Platform::Collections::Vector<Platform::String ^>();
    for (size_t i = 0; i < commands.size(); i++) {
        speechCommandList->Append(Platform::StringReference(commands[i].c_str()));
    }

    SpeechRecognitionListConstraint ^ spConstraint = ref new SpeechRecognitionListConstraint(speechCommandList);
    m_speechRecognizer->Constraints->Clear();
    m_speechRecognizer->Constraints->Append(spConstraint);

    m_host->runInBackground([this]() {
        auto compileAsync = m_speechRecognizer->CompileConstraintsAsync();
        create_task(compileAsync).wait();
        auto result = compileAsync->GetResults();
        if (result->Status == SpeechRecognitionResultStatus::Success) {
            auto startAsync = m_speechRecognizer->ContinuousRecognitionSession->StartAsync();
            create_task(startAsync).wait();
        }
        return S_OK;
    });
}