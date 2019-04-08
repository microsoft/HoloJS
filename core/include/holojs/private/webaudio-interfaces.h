#pragma once

#include "chakra.h"
#include "event-target.h"
#include "object-lifetime.h"

namespace HoloJs {
class IHoloJsScriptHostInternal;

class IAudioBuffer : public HoloJs::ResourceManagement::IRelease {
};

class IAudioNode : public HoloJs::ResourceManagement::IRelease {
   public:
    virtual JsValueRef connect(IAudioNode* destination) = 0;

    virtual JsValueRef disconnect() = 0;
};

class IAudioParam : public IAudioNode {
   public:
    virtual void setValueAtTime(float value, double time) = 0;
    virtual void setTargetAtTime(double target, double startTime, double timeConstant) = 0;

	virtual float getValue() = 0;
	virtual void setValue(float value) = 0;

	virtual float getDefaultValue() = 0;
	virtual float getMinValue() = 0;
	virtual float getMaxValue() = 0;

	virtual void linearRampToValueAtTime(float value, float time) = 0;
	virtual void exponentialRampToValueAtTime(float value, float time) = 0;

	virtual void setValueCurveAtTime(const std::vector<float>& values, float time, float duration) = 0;

	virtual void cancelScheduledValues(float startTime) = 0;
};

class IGainNode : public IAudioNode {
};

class IPannerNode : public IAudioNode {
   public:
    virtual void setPosition(float x, float y, float z) = 0;
    virtual void setOrientation(float x, float y, float z) = 0;
    virtual void setVelocity(float x, float y, float z) = 0;
	virtual void setRefDistance(float refDistance) = 0;
	virtual float getRefDistance() = 0;

	virtual std::wstring getPanningModel() = 0;
	virtual void setPanningModel(const std::wstring& panningModel) = 0;
	virtual std::wstring getDistanceModel() = 0;
	virtual void setDistanceModel(const std::wstring& distanceModel) = 0;

	virtual IAudioParam* positionX() = 0;
	virtual IAudioParam* positionY() = 0;
	virtual IAudioParam* positionZ() = 0;

	virtual IAudioParam* orientationX() = 0;
	virtual IAudioParam* orientationY() = 0;
	virtual IAudioParam* orientationZ() = 0;

	virtual float getMaxDistance() = 0;
	virtual void setMaxDistance(float maxDistance) = 0;

	virtual float getRolloffFactor() = 0;
	virtual void setRolloffFactor(float rolloffFactor) = 0;

	virtual float getConeInnerAngle() = 0;
	virtual void setConeInnerAngle(float innerAngle) = 0;

	virtual float getConeOuterAngle() = 0;
	virtual void setConeOuterAngle(float outerAngle) = 0;

	virtual float getConeOuterGain() = 0;
	virtual void setConeOuterGain(float outerGain) = 0;
};

class IAudioListener : public IAudioNode {
   public:
    virtual void setPosition(float x, float y, float z) = 0;
    virtual void setOrientation(float x, float y, float z, float upX, float upY, float upZ) = 0;
	virtual IAudioParam* positionX() = 0;
	virtual IAudioParam* positionY() = 0;
	virtual IAudioParam* positionZ() = 0;
	
	virtual IAudioParam* forwardX() = 0;
	virtual IAudioParam* forwardY() = 0;
	virtual IAudioParam* forwardZ() = 0;

	virtual IAudioParam* upX() = 0;
	virtual IAudioParam* upY() = 0;
	virtual IAudioParam* upZ() = 0;
};

class IAudioDestinationNode : public IAudioNode {
};

class IAudioBufferSource : public IAudioNode, public HoloJs::EventTarget {
   public:
    virtual IAudioParam* getPlaybackRate() = 0;
	virtual IAudioParam* getDetune() = 0;
    virtual void start(float when) = 0;
    virtual void stop(float when) = 0;
    virtual void setBuffer(IAudioBuffer* buffer) = 0;

	virtual bool getLoop() = 0;
	virtual void setLoop(bool value) = 0;

	virtual double getLoopStart() = 0;
	virtual void setLoopStart(double value) = 0;

	virtual double getLoopEnd() = 0;
	virtual void setLoopEnd(double value) = 0;
};

class IAudioContext : public HoloJs::ResourceManagement::IRelease {
   public:
    virtual ~IAudioContext() {}

    virtual IGainNode* createGain() = 0;

    virtual IAudioDestinationNode* getDestination() = 0;
    virtual IPannerNode* createPanner() = 0;
    virtual IAudioListener* getListener() = 0;
    virtual IAudioBufferSource* createAudioBufferSource() = 0;

    virtual long decodeAudioData(JsValueRef data, JsValueRef onSuccessCallback, JsValueRef onFailedCallback) = 0;

	virtual float getSampleRate() = 0;
	virtual double getCurrentTime() = 0;
};

}  // namespace HoloJs