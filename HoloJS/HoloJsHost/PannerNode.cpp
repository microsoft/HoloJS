#include "pch.h"
#include "PannerNode.h"
#include "ScriptHostUtilities.h"
#include "ScriptResourceTracker.h"

using namespace HologramJS::Audio;
using namespace HologramJS::Utilities;

bool PannerNode::InitializeProjections()
{
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setPosition", L"pannerNode", setPosition));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setOrientation", L"pannerNode", setOrientation));
    RETURN_IF_FALSE(ScriptHostUtilities::ProjectFunction(L"setVelocity", L"pannerNode", setVelocity));

    return true;
}

JsValueRef CHAKRA_CALLBACK PannerNode::setPosition(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_IF_FALSE(argumentCount == 5);

    auto pannerNodeUnknown = ScriptResourceTracker::ExternalToObject<PannerNode>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(pannerNodeUnknown);

    auto pannerNode = dynamic_cast<PannerNode*>(pannerNodeUnknown);

    const auto x = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    const auto y = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    const auto z = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);

    pannerNode->m_pannerNode->setPosition(x, y, z);

    return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK PannerNode::setOrientation(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount < 3);

    auto pannerNodeUnknown = ScriptResourceTracker::ExternalToObject<PannerNode>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(pannerNodeUnknown);

    auto pannerNode = dynamic_cast<PannerNode*>(pannerNodeUnknown);

    const auto x = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    const auto y = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    const auto z = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);
    pannerNode->m_pannerNode->setOrientation(x, y, z);

    return JS_INVALID_REFERENCE;
}

JsValueRef CHAKRA_CALLBACK PannerNode::setVelocity(
    JsValueRef callee, bool isConstructCall, JsValueRef* arguments, unsigned short argumentCount, PVOID callbackData)
{
    RETURN_INVALID_REF_IF_TRUE(argumentCount < 3);

    auto pannerNodeUnknown = ScriptResourceTracker::ExternalToObject<PannerNode>(arguments[1]);
    RETURN_INVALID_REF_IF_NULL(pannerNodeUnknown);

    auto pannerNode = dynamic_cast<PannerNode*>(pannerNodeUnknown);

    const auto x = ScriptHostUtilities::GLfloatFromJsRef(arguments[2]);
    const auto y = ScriptHostUtilities::GLfloatFromJsRef(arguments[3]);
    const auto z = ScriptHostUtilities::GLfloatFromJsRef(arguments[4]);

    pannerNode->m_pannerNode->setVelocity(x, y, z);

    return JS_INVALID_REFERENCE;
}
