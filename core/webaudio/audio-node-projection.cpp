#include "audio-node-projection.h"
#include "../host-interfaces.h"
#include "../include/holojs/private/platform-interfaces.h"
#include "../include/holojs/private/script-host-utilities.h"
#include "../include/holojs/private/webaudio-interfaces.h"
#include "../resource-management/resource-manager.h"

using namespace HoloJs;
using namespace HoloJs::ResourceManagement;
using namespace HoloJs::WebAudio;
using namespace std;

HRESULT AudioNodeProjection::initialize()
{
    // WebGL context projections
    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"audioNodeConnect", L"webaudio", connectStatic, this, &m_connectFunction));

    RETURN_IF_FAILED(ScriptHostUtilities::ProjectFunction(
        L"audioNodeDisconnect", L"webaudio", disconnectStatic, this, &m_disconnectFunction));

    return S_OK;
}

JsValueRef AudioNodeProjection::connect(JsValueRef* arguments, unsigned short argumentCount)
{
	RETURN_INVALID_REF_IF_TRUE(argumentCount < 3);

    auto audioNode = m_resourceManager->externalToObject<IAudioNode>(arguments[1], ObjectType::IAudioNode);
    RETURN_INVALID_REF_IF_NULL(audioNode);

    auto destinationNode = m_resourceManager->externalToObject<IAudioNode>(arguments[2], ObjectType::IAudioNode);
    RETURN_INVALID_REF_IF_NULL(destinationNode);

	audioNode->connect(destinationNode);

	return arguments[2];
}

JsValueRef AudioNodeProjection::disconnect(JsValueRef* arguments, unsigned short argumentCount)
{
    RETURN_INVALID_REF_IF_FALSE(argumentCount == 2);

    auto audioNode = m_resourceManager->externalToObject<IAudioNode>(arguments[1], ObjectType::IAudioNode);
    RETURN_INVALID_REF_IF_NULL(audioNode);

    audioNode->disconnect();

    return JS_INVALID_REFERENCE;
}