#include "speech-recognizer-projection.h"
#include "blob.h"
#include "include/holojs/private/error-handling.h"
#include "include/holojs/private/platform-interfaces.h"
#include "include/holojs/private/script-host-utilities.h"
#include "include/holojs/private/speech-recognizer.h"
#include "resource-management/external-object.h"

#include <string>
#include <vector>

using namespace HoloJs;
using namespace HoloJs::ResourceManagement;
using namespace std;

long SpeechRecognizerProjection::initialize()
{
    JS_PROJECTION_REGISTER(L"SpeechRecognizer", L"isAvailable", isAvailable);
    JS_PROJECTION_REGISTER(L"SpeechRecognizer", L"create", create);
    JS_PROJECTION_REGISTER(L"SpeechRecognizer", L"start", start);
    JS_PROJECTION_REGISTER(L"SpeechRecognizer", L"stop", stop);
    JS_PROJECTION_REGISTER(L"SpeechRecognizer", L"setKeywords", setKeywords);
    JS_PROJECTION_REGISTER(L"SpeechRecognizer", L"enableFreeFormDictation", enableFreeFormDictation);

    return HoloJs::Success;
}

JsValueRef SpeechRecognizerProjection::_isAvailable(JsValueRef* arguments, unsigned short argumentCount)
{
    auto speechRecognizerAvailable = getPlatform()->isSpeechRecognizerAvailable() && m_isEnabled;
    JsValueRef speechRecognizerAvailableRef;
    RETURN_INVALID_REF_IF_JS_ERROR(JsBoolToBoolean(speechRecognizerAvailable, &speechRecognizerAvailableRef));

    return speechRecognizerAvailableRef;
}

JsValueRef SpeechRecognizerProjection::_create(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    RETURN_INVALID_REF_IF_FALSE(m_isEnabled);

    auto newSpeechRecognizer = getPlatform()->getSpeechRecognizer(m_host);
    RETURN_INVALID_REF_IF_NULL(newSpeechRecognizer);

    auto speechRecognizerExternal =
        m_resourceManager->objectToDirectExternal(newSpeechRecognizer, ObjectType::ISpeechRecognizer);

    RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::SetJsProperty(arguments[1], L"native", speechRecognizerExternal));

    return JS_INVALID_REFERENCE;
}

JsValueRef SpeechRecognizerProjection::_start(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto speechRecognizer =
        m_resourceManager->externalToObject<ISpeechRecognizer>(arguments[1], ObjectType::ISpeechRecognizer);
    RETURN_INVALID_REF_IF_NULL(speechRecognizer);

    speechRecognizer->start(arguments[1]);

    return JS_INVALID_REFERENCE;
}

JsValueRef SpeechRecognizerProjection::_stop(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto speechRecognizer =
        m_resourceManager->externalToObject<ISpeechRecognizer>(arguments[1], ObjectType::ISpeechRecognizer);
    RETURN_INVALID_REF_IF_NULL(speechRecognizer);

    speechRecognizer->stop();

    return JS_INVALID_REFERENCE;
}

JsValueRef SpeechRecognizerProjection::_setKeywords(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto speechRecognizer =
        m_resourceManager->externalToObject<ISpeechRecognizer>(arguments[1], ObjectType::ISpeechRecognizer);
    RETURN_INVALID_REF_IF_NULL(speechRecognizer);

    JsValueType keywordsArrayType;

    RETURN_INVALID_REF_IF_JS_ERROR(JsGetValueType(arguments[2], &keywordsArrayType));
    RETURN_INVALID_REF_IF_FALSE(keywordsArrayType == JsArray);

    // Get the length of the parts array
    JsValueRef lengthRef;
    RETURN_NULL_IF_FAILED(ScriptHostUtilities::GetJsProperty(arguments[2], L"length", &lengthRef));
    int length = ScriptHostUtilities::GLintFromJsRef(lengthRef);

    vector<wstring> keywords;

    // Enumerate all the parts, sum theirs sizes and obtain pointers to their data
    for (int i = 0; i < length; i++) {
        JsValueRef indexRef;
        RETURN_NULL_IF_JS_ERROR(JsIntToNumber(i, &indexRef));

        JsValueRef keywordRef;
        RETURN_INVALID_REF_IF_JS_ERROR(JsGetIndexedProperty(arguments[2], indexRef, &keywordRef));

        JsValueType keywordRefType;
        RETURN_INVALID_REF_IF_JS_ERROR(JsGetValueType(keywordRef, &keywordRefType));

        RETURN_INVALID_REF_IF_FALSE(keywordRefType == JsString);

        wstring keyword;
        RETURN_INVALID_REF_IF_FAILED(ScriptHostUtilities::GetString(keywordRef, keyword));
        keywords.emplace_back(keyword);
    }

    speechRecognizer->setKeywords(keywords);

    return JS_INVALID_REFERENCE;
}

JsValueRef SpeechRecognizerProjection::_enableFreeFormDictation(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 3);

    auto speechRecognizer =
        m_resourceManager->externalToObject<ISpeechRecognizer>(arguments[1], ObjectType::ISpeechRecognizer);
    RETURN_INVALID_REF_IF_NULL(speechRecognizer);

    bool enableValue;
    RETURN_INVALID_REF_IF_JS_ERROR(JsBooleanToBool(arguments[2], &enableValue));

    speechRecognizer->enableFreeFormDictation(enableValue);

    return JS_INVALID_REFERENCE;
}