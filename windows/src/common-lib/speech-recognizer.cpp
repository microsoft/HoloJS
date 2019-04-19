#include "stdafx.h"
#include "holojs/private/error-handling.h"
#include "holojs/private/holojs-script-host-internal.h"
#include "holojs/private/script-host-utilities.h"
#include "include/holojs/windows/speech-recognizer.h"
#include <collection.h>
#include <ppltasks.h>
#include <string>
#include <vector>

using namespace concurrency;
using namespace HoloJs::Platforms::Win32;
using namespace Windows::Media;
using namespace Windows::Foundation;
using namespace std;
using namespace std::placeholders;

SpeechRecognizer::~SpeechRecognizer() { stop(); }

long SpeechRecognizer::start()
{
    RETURN_IF_TRUE(m_isStarted);

    m_isStarted = true;

    if (m_speechRecognizer != nullptr) {
        m_speechRecognizer->ContinuousRecognitionSession->ResultGenerated -= m_resultGeneratedToken;
        m_speechRecognizer = nullptr;
    }

    m_speechRecognizer = ref new Windows::Media::SpeechRecognition::SpeechRecognizer();

    m_resultGeneratedToken = m_speechRecognizer->ContinuousRecognitionSession->ResultGenerated +=
        ref new TypedEventHandler<SpeechRecognition::SpeechContinuousRecognitionSession ^,
                                  SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^>(
            [this](SpeechRecognition::SpeechContinuousRecognitionSession ^ session,
                   SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^ args) {
                onResultGenerated(session, args);
            });

    create_task([this]() {
        Platform::Collections::Vector<Platform::String ^> ^ speechCommandList =
            ref new Platform::Collections::Vector<Platform::String ^>();
        for (const auto& keyword : m_keywords) {
            speechCommandList->Append(Platform::StringReference(keyword.c_str()));
        }

        SpeechRecognition::SpeechRecognitionListConstraint ^ spConstraint =
            ref new SpeechRecognition::SpeechRecognitionListConstraint(speechCommandList);
        m_speechRecognizer->Constraints->Clear();
        m_speechRecognizer->Constraints->Append(spConstraint);

        auto compileAsync = m_speechRecognizer->CompileConstraintsAsync();
        create_task(compileAsync).then([this](SpeechRecognition::SpeechRecognitionCompilationResult ^ result) {
            if (result->Status == SpeechRecognition::SpeechRecognitionResultStatus::Success) {
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

long SpeechRecognizer::start(JsValueRef mapperRef)
{
    RETURN_IF_JS_ERROR(JsAddRef(mapperRef, nullptr));

    m_mapperRef = mapperRef;

    return start();
}

long SpeechRecognizer::stop()
{
    if (m_mapperRef != JS_INVALID_REFERENCE) {
        JsRelease(m_mapperRef, nullptr);
    }
    
    if (m_speechRecognizer != nullptr) {
        m_speechRecognizer->ContinuousRecognitionSession->ResultGenerated -= m_resultGeneratedToken;
        m_speechRecognizer = nullptr;
    }

    return S_OK;
}

void SpeechRecognizer::onResultGenerated(SpeechRecognition::SpeechContinuousRecognitionSession ^ sender,
                                         SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^ args)
{
    wstring text = args->Result->Text->Data();
    double confidence = args->Result->RawConfidence;

    const bool handled = m_onResultsCallback ? (*m_onResultsCallback.get())(text, confidence) : false;

    if (!handled && m_mapperRef != JS_INVALID_REFERENCE) {
        m_host->runInScriptContext([this, text, confidence]() {
            JsValueRef speechEvent;
            EXIT_IF_JS_ERROR(JsCreateObject(&speechEvent));
            EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(speechEvent, L"text", text));
            EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(speechEvent, L"confidence", confidence));

            EXIT_IF_FAILED(invokeEventListeners(L"result", speechEvent));
        });
    }
}
