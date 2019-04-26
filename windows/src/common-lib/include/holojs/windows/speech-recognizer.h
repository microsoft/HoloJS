#pragma once

#include "holojs/private/chakra.h"
#include "holojs/private/speech-recognizer.h"
#include <functional>
#include <memory>
#include <mutex>

namespace HoloJs {
class IHoloJsScriptHostInternal;
}

namespace HoloJs {
namespace Platforms {
namespace Win32 {

class SpeechRecognizer : public HoloJs::ISpeechRecognizer, public HoloJs::EventTarget {
   public:
    SpeechRecognizer(HoloJs::IHoloJsScriptHostInternal* host) : m_host(host) {}
    SpeechRecognizer() {}
    ~SpeechRecognizer();

    virtual void Release() {}

    virtual long start(JsValueRef mapperRef);
    long start();
    virtual long stop();

    virtual void enableFreeFormDictation(bool enabled) {}
    virtual void setKeywords(const std::vector<std::wstring>& keywords) { m_keywords = keywords; }

    void setOnResultsCallback(OnSpeechRecognizerResult callback)
    {
        m_onResultsCallback = std::make_unique<OnSpeechRecognizerResult>(callback);
    }

   private:
    Windows::Media::SpeechRecognition::SpeechRecognizer ^ m_speechRecognizer;

    Windows::Foundation::EventRegistrationToken m_resultGeneratedToken;
    Windows::Foundation::EventRegistrationToken m_recognitionCompleteToken;

    HRESULT compileKeywords(const std::vector<std::wstring>& commands);

    HoloJs::IHoloJsScriptHostInternal* m_host = nullptr;
    JsValueRef m_mapperRef = JS_INVALID_REFERENCE;
    bool m_isStarted = false;

    std::unique_ptr<OnSpeechRecognizerResult> m_onResultsCallback;

    std::vector<std::wstring> m_keywords;

    void invokeErrorCallback(int code, const std::wstring& message);

    class SpeechRecognizerContext {
       public:
        SpeechRecognizerContext(SpeechRecognizer* recognizer) : m_recognizer(recognizer) {}

        ~SpeechRecognizerContext()
        {
            std::lock_guard<std::mutex> guard(m_contextLock);
            m_isReleased = true;
        }

        void lock() { m_contextLock.lock(); }
        void unlock() { m_contextLock.unlock(); }

        bool isReleased() const { return m_isReleased; }
        SpeechRecognizer* getRecognizer() { return m_recognizer; }

       private:
        bool m_isReleased = false;
        std::mutex m_contextLock;
        SpeechRecognizer* m_recognizer;
    };

    class LockedSpeechRecognizerContext {
       public:
        LockedSpeechRecognizerContext(std::shared_ptr<SpeechRecognizerContext> context)
        {
			m_context = context;
			context->lock();
        }

        ~LockedSpeechRecognizerContext() { m_context->unlock(); }

		bool isReleased() { return m_context->isReleased(); }

		SpeechRecognizer* getRecognizer() { return m_context->getRecognizer(); }

       private:
        std::shared_ptr<SpeechRecognizerContext> m_context;
    };

    static std::unique_ptr<LockedSpeechRecognizerContext> lockContext(
        std::weak_ptr<SpeechRecognizerContext> weakContext)
    {
        auto context = weakContext.lock();
        if (context) {
            return std::make_unique<LockedSpeechRecognizerContext>(context);
        } else {
            return nullptr;
        }
    }

    std::shared_ptr<SpeechRecognizerContext> m_context;

    static void onResultGenerated(
        std::weak_ptr<SpeechRecognizerContext> context,
        Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^ sender,
        Windows::Media::SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^ args);

    static void onCompleted(std::weak_ptr<SpeechRecognizerContext> context,
                            Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^ sender,
                            Windows::Media::SpeechRecognition::SpeechContinuousRecognitionCompletedEventArgs ^ args);
};

}  // namespace Win32
}  // namespace Platforms
}  // namespace HoloJs
