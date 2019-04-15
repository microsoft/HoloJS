#pragma once

#include "LabSound/extended/LabSound.h"
#include "audio-node.h"
#include "holojs/private/chakra.h"
#include "holojs/private/object-lifetime.h"
#include "holojs/private/webaudio-interfaces.h"

namespace HoloJs {

class HoloJsScriptHost;

namespace Win32 {

namespace WebAudio {

class PannerNode : public HoloJs::IPannerNode, public AudioNode {
   public:
    PannerNode(std::shared_ptr<lab::AudioContext> context, std::shared_ptr<lab::PannerNode> pannerNode)
        : m_pannerNode(pannerNode), m_context(context), AudioNode(context, pannerNode)
    {
    }

    virtual ~PannerNode() { OutputDebugString(L"G"); }

    virtual void Release() {}

    virtual JsValueRef connect(IAudioNode* destination) { return AudioNode::audioNodeConnect(destination); }

    virtual JsValueRef disconnect() { return AudioNode::audioNodeDisconnect(); }

    virtual void setPosition(float x, float y, float z) { m_pannerNode->setPosition(x, y, z); }
    virtual void setOrientation(float x, float y, float z) { m_pannerNode->setOrientation(lab::FloatPoint3D(x, y, z)); }
    virtual void setVelocity(float x, float y, float z) { m_pannerNode->setVelocity(x, y, z); }

    virtual void setRefDistance(float refDistance) { m_pannerNode->setRefDistance(refDistance); }
    virtual float getRefDistance() { return m_pannerNode->refDistance(); }

    virtual std::wstring getPanningModel()
    {
        // TODO: use m_pannerNode->panningModel() after LabSound fix for https://github.com/LabSound/LabSound/issues/120
        switch (m_panningMode) {
            case lab::PanningMode::HRTF:
                return L"HRTF";

            case lab::PanningMode::EQUALPOWER:
                return L"equalpower";

            default:
                return L"";
        };

        return L"";
    }

    virtual void setPanningModel(const std::wstring& panningModel)
    {
        if (_wcsicmp(panningModel.c_str(), L"HRTF") == 0) {
            m_pannerNode->setPanningModel(lab::PanningMode::HRTF);
            // TODO: Remove this after fix for https://github.com/LabSound/LabSound/issues/120
            m_panningMode = lab::PanningMode::HRTF;
        } else if (_wcsicmp(panningModel.c_str(), L"equalpower") == 0) {
            m_pannerNode->setPanningModel(lab::PanningMode::EQUALPOWER);
            // TODO: Remove this after firx for https://github.com/LabSound/LabSound/issues/120
            m_panningMode = lab::PanningMode::EQUALPOWER;
        }
    }

    virtual std::wstring getDistanceModel()
    {
        switch (m_pannerNode->distanceModel()) {
            case lab::PannerNode::DistanceModel::LINEAR_DISTANCE:
                return L"linear";

            case lab::PannerNode::DistanceModel::INVERSE_DISTANCE:
                return L"inverse";

            case lab::PannerNode::DistanceModel::EXPONENTIAL_DISTANCE:
                return L"exponential";

            default:
                return L"";
        }
    }

    virtual void setDistanceModel(const std::wstring& distanceModel)
    {
        if (_wcsicmp(distanceModel.c_str(), L"linear")) {
            m_pannerNode->setDistanceModel(lab::PannerNode::DistanceModel::LINEAR_DISTANCE);
        } else if (_wcsicmp(distanceModel.c_str(), L"inverse")) {
            m_pannerNode->setDistanceModel(lab::PannerNode::DistanceModel::INVERSE_DISTANCE);
        } else if (_wcsicmp(distanceModel.c_str(), L"exponential")) {
            m_pannerNode->setDistanceModel(lab::PannerNode::DistanceModel::EXPONENTIAL_DISTANCE);
        }
    }

    virtual IAudioParam* positionX() { return new AudioParam(m_context, m_pannerNode->positionX()); }
    virtual IAudioParam* positionY() { return new AudioParam(m_context, m_pannerNode->positionY()); }
    virtual IAudioParam* positionZ() { return new AudioParam(m_context, m_pannerNode->positionZ()); }

    virtual IAudioParam* orientationX() { return new AudioParam(m_context, m_pannerNode->orientationX()); }
    virtual IAudioParam* orientationY() { return new AudioParam(m_context, m_pannerNode->orientationY()); }
    virtual IAudioParam* orientationZ() { return new AudioParam(m_context, m_pannerNode->orientationZ()); }

    virtual float getMaxDistance() { return m_pannerNode->maxDistance(); }
    virtual void setMaxDistance(float maxDistance) { return m_pannerNode->setMaxDistance(maxDistance); }

    virtual float getRolloffFactor() { return m_pannerNode->rolloffFactor(); }
    virtual void setRolloffFactor(float rolloffFactor) { return m_pannerNode->setRolloffFactor(rolloffFactor); }

    virtual float getConeInnerAngle() { return m_pannerNode->coneInnerAngle(); }
    virtual void setConeInnerAngle(float innerAngle) { return m_pannerNode->setConeInnerAngle(innerAngle); }

    virtual float getConeOuterAngle() { return m_pannerNode->coneOuterAngle(); }
    virtual void setConeOuterAngle(float outerAngle) { return m_pannerNode->setConeOuterAngle(outerAngle); }

    virtual float getConeOuterGain() { return m_pannerNode->coneOuterGain(); }
    virtual void setConeOuterGain(float outerGain) { return m_pannerNode->setConeOuterGain(outerGain); }

   private:
    std::shared_ptr<lab::AudioContext> m_context;
    lab::PanningMode m_panningMode;
    std::shared_ptr<lab::PannerNode> m_pannerNode;
};
}  // namespace WebAudio
}  // namespace Win32
}  // namespace HoloJs