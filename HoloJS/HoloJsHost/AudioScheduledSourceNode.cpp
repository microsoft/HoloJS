#include "pch.h"
#include "AudioScheduledSourceNode.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"

using namespace HologramJS::Audio;
using namespace HologramJS::Utilities;

bool AudioScheduledSourceNode::InitializeProjections()
{
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"start", L"audioScheduledSourceNode", start));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"stop", L"audioScheduledSourceNode", stop));

    return true;
}

JsValueRef CHAKRA_CALLBACK AudioScheduledSourceNode::start(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount < 2);

    auto scheduledSourceNode = ScriptResourceTracker::ExternalToObject<AudioScheduledSourceNode>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(scheduledSourceNode);

    float when = argumentCount > 2 ? ScriptHostUtilities::GLfloatFromJsRef(arguments[2]) : 0;

    scheduledSourceNode->m_node->start(when);

    return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK AudioScheduledSourceNode::stop(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount < 2);

    auto scheduledSourceNode = ScriptResourceTracker::ExternalToObject<AudioScheduledSourceNode>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(scheduledSourceNode);

    float when = argumentCount > 2 ? ScriptHostUtilities::GLfloatFromJsRef(arguments[2]) : 0;

    scheduledSourceNode->m_node->stop(when);

    return JS_INVALID_REFERENCE;
}
