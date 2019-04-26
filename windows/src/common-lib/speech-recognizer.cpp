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

    if (!m_context) {
        m_context = make_shared<SpeechRecognizerContext>(this);
        weak_ptr<SpeechRecognizerContext> capturedContext = m_context;

        m_speechRecognizer = ref new Windows::Media::SpeechRecognition::SpeechRecognizer();

        m_resultGeneratedToken = m_speechRecognizer->ContinuousRecognitionSession->ResultGenerated +=
            ref new TypedEventHandler<SpeechRecognition::SpeechContinuousRecognitionSession ^,
                                      SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^>(
                [capturedContext](SpeechRecognition::SpeechContinuousRecognitionSession ^ session,
                                  SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^ args) {
                    onResultGenerated(capturedContext, session, args);
                });

        m_recognitionCompleteToken = m_speechRecognizer->ContinuousRecognitionSession->Completed +=
            ref new Windows::Foundation::TypedEventHandler<
                Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^
                , Windows::Media::SpeechRecognition::SpeechContinuousRecognitionCompletedEventArgs ^>(
                [capturedContext](Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^ session,
                                  Windows::Media::SpeechRecognition::SpeechContinuousRecognitionCompletedEventArgs ^
                                      args) { onCompleted(capturedContext, session, args); });
    }

    m_host->runInBackground([this]() -> HRESULT {
        if (FAILED(compileKeywords(m_keywords))) {
            invokeErrorCallback(-1, L"failed to compile keywords");
        } else {
            try {
                auto startAsync = m_speechRecognizer->ContinuousRecognitionSession->StartAsync();
                create_task(startAsync).wait();
				return S_OK;
            } catch (Platform::Exception ^ e) {
                int code = e->HResult;
                wstring message = e->Message->Data();
                invokeErrorCallback(e->HResult, message);
            }
        }

		m_isStarted = false;
        return E_FAIL;
    });

    return S_OK;
}

HRESULT SpeechRecognizer::compileKeywords(const vector<wstring>& keywords)
{
    if (keywords.size() > 0) {
        Platform::Collections::Vector<Platform::String ^> ^ speechCommandList =
            ref new Platform::Collections::Vector<Platform::String ^>();
        for (const auto& keyword : keywords) {
            speechCommandList->Append(Platform::StringReference(keyword.c_str()));
        }

        SpeechRecognition::SpeechRecognitionListConstraint ^ spConstraint =
            ref new SpeechRecognition::SpeechRecognitionListConstraint(speechCommandList);
        m_speechRecognizer->Constraints->Clear();
        m_speechRecognizer->Constraints->Append(spConstraint);
    } else {
        m_speechRecognizer->Constraints->Clear();
    }

    try {
        auto compileAsync = m_speechRecognizer->CompileConstraintsAsync();
        create_task(compileAsync).wait();
    } catch (...) {
        return E_FAIL;
    }

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
    if (m_speechRecognizer != nullptr && m_isStarted) {
        m_speechRecognizer->ContinuousRecognitionSession->StopAsync();
    }

    return S_OK;
}

void SpeechRecognizer::onResultGenerated(std::weak_ptr<SpeechRecognizerContext> context,
                                         SpeechRecognition::SpeechContinuousRecognitionSession ^ sender,
                                         SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^ args)
{
    auto capturedContext = lockContext(context);
    EXIT_IF_TRUE(!capturedContext || capturedContext->isReleased());
    auto recognizer = capturedContext->getRecognizer();

    wstring text = args->Result->Text->Data();
    double confidence = args->Result->RawConfidence;

    const bool handled =
        recognizer->m_onResultsCallback ? (*recognizer->m_onResultsCallback.get())(text, confidence) : false;

    if (!handled && recognizer->m_mapperRef != JS_INVALID_REFERENCE) {
        recognizer->m_host->runInScriptContext([context, text, confidence]() {
            auto capturedContext = lockContext(context);
            EXIT_IF_TRUE(!capturedContext || capturedContext->isReleased());
            auto recognizer = capturedContext->getRecognizer();

            JsValueRef speechEvent;
            EXIT_IF_JS_ERROR(JsCreateObject(&speechEvent));
            EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(speechEvent, L"text", text));
            EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(speechEvent, L"confidence", confidence));

            EXIT_IF_FAILED(recognizer->invokeEventListeners(L"result", speechEvent));
        });
    }
}

void SpeechRecognizer::invokeErrorCallback(int code, const wstring& message)
{
    m_host->runInScriptContext([this, code, message]() {
        JsValueRef errorRef;
        EXIT_IF_JS_ERROR(JsCreateObject(&errorRef));

        EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(errorRef, L"code", code));
        EXIT_IF_FAILED(ScriptHostUtilities::SetJsProperty(errorRef, L"message", message));

        invokeEventListeners(L"error", errorRef);
    });
}

void SpeechRecognizer::onCompleted(std::weak_ptr<SpeechRecognizerContext> context,
                                   Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^ sender,
                                   Windows::Media::SpeechRecognition::SpeechContinuousRecognitionCompletedEventArgs ^
                                       args)
{
    auto capturedContext = lockContext(context);
    EXIT_IF_TRUE(!capturedContext || capturedContext->isReleased());
    auto recognizer = capturedContext->getRecognizer();

    recognizer->m_isStarted = false;

    recognizer->m_host->runInScriptContext([context]() {
        auto capturedContext = lockContext(context);
        EXIT_IF_TRUE(!capturedContext || capturedContext->isReleased());
        auto recognizer = capturedContext->getRecognizer();

        if (recognizer->m_mapperRef != JS_INVALID_REFERENCE) {
            JsRelease(recognizer->m_mapperRef, nullptr);
            recognizer->m_mapperRef = JS_INVALID_REFERENCE;
        }

        recognizer->invokeEventListeners(L"stop");
    });
}
