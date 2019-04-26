#pragma once

#include "chakra.h"
#include "event-target.h"
#include "holojs-view.h"
#include "object-lifetime.h"

namespace HoloJs {

typedef std::function<bool(const std::wstring& text, double confidence)> OnSpeechRecognizerResult;

class ISpeechRecognizer : public HoloJs::ResourceManagement::IRelease {
   public:
    virtual ~ISpeechRecognizer() {}

    virtual long start(JsValueRef recognizerRef) = 0;
	virtual long stop() = 0;

	virtual void enableFreeFormDictation(bool enabled) = 0;
	virtual void setKeywords(const std::vector<std::wstring>& keywords) = 0;

	virtual void setOnResultsCallback(OnSpeechRecognizerResult callback) = 0;
};

}  // namespace HoloJs