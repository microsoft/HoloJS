#pragma once

#include "holojs/private/chakra.h"
#include "holojs/private/speech-recognizer.h"
#include <memory>
#include <functional>

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

    void onResultGenerated(Windows::Media::SpeechRecognition::SpeechContinuousRecognitionSession ^ sender,
                           Windows::Media::SpeechRecognition::SpeechContinuousRecognitionResultGeneratedEventArgs ^
                               args);

    Windows::Foundation::EventRegistrationToken m_resultGeneratedToken;

    void compileKeywordsAsync(std::vector<std::wstring>& commands);

    HoloJs::IHoloJsScriptHostInternal* m_host = nullptr;
    JsValueRef m_mapperRef = JS_INVALID_REFERENCE;
    bool m_isStarted = false;

    std::unique_ptr<OnSpeechRecognizerResult> m_onResultsCallback;

    std::vector<std::wstring> m_keywords;
};

}  // namespace Win32
}  // namespace Platforms
}  // namespace HoloJs
