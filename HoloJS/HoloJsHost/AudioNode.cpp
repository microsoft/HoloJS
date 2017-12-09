#include "pch.h"
#include "AudioNode.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"

using namespace HologramJS::Audio;
using namespace HologramJS::Utilities;

AudioNode::~AudioNode() {}

bool AudioNode::InitializeProjections()
{
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"connect", L"audioNode", connect));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"disconnect", L"audioNode", disconnect));

    return true;
}

JsValueRef CHAKRA_CALLBACK AudioNode::connect(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount < 3);

    auto audioNodeUnknown = ScriptResourceTracker::ExternalToObject<AudioNode>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(audioNodeUnknown);

    auto audioNode = dynamic_cast<AudioNode*>(audioNodeUnknown);

    auto destinationNode = ScriptResourceTracker::ExternalToObject<AudioNode>(arguments[2]);
    RETURN_INVALID_REF_IF_NULL(destinationNode);

    audioNode->m_node->connect(audioNode->m_context.get(), destinationNode->m_node.get(), 0, 0);

    return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK AudioNode::disconnect(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount < 2);

    auto audioNode = ScriptResourceTracker::ExternalToObject<AudioNode>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(audioNode);

    return JS_INVALID_REFERENCE;
}
