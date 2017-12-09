window.AudioContext = function () {

    this.nativeContext = holographic.nativeInterface.audio.createContext();

    this.destination = new AudioDestinationNode(this);

    this.createGain = function () {
        return new GainNode(this);
    };

    this.createPanner = function () {
        return new PannerNode(this);
    };

    this.listener = new AudioListener();

    this.decodeAudioData = function (data, success, error) {
        holographic.nativeInterface.audio.decodeAudioData(
            this.nativeContext,
            data,
            function (nativeSoundBuffer) {
                if (success) success(new SoundBuffer(nativeSoundBuffer));
            },
            error);
    };

    this.createBufferSource = function () {
        return new AudioBufferSourceNode(this);
    };

    console.log("Created audio context");
};

function AudioNode(audioContext) {
    this.connect = function (destination, outputIndex, inputIndex) {
        holographic.nativeInterface.audioNode.connect(this.nativeAudioNode, destination.nativeAudioNode, outputIndex, inputIndex);
        console.log("audio node connect called");
    };

    this.disconnect = function (destination, output, input) {
        holographic.nativeInterface.audioNode.disconnect(this.nativeAudioNode, destination, output, input);
    };

    this.context = audioContext;
}

function GainNode(audioContext) {
    AudioNode.call(this, audioContext);
    this.nativeAudioNode = holographic.nativeInterface.audioContext.createGain(audioContext.nativeContext);
}

function PannerNode(audioContext) {
    AudioNode.call(this, audioContext);
    this.nativeAudioNode = holographic.nativeInterface.audioContext.createPanner(audioContext.nativeContext);

    this.setPosition = function () {
        console.log("panner set position");
    };

    this.setOrientation = function () {
        console.log("panner set orientation");
    };

    this.setVelocity = function () {
        console.log("panner set velocity");
    };
}

function AudioListener(audioContext) {
    AudioNode.call(this, audioContext);

    this.setPosition = function () {
        console.log("panner set position");
    };

    this.setOrientation = function () {
        console.log("panner set orientation");
    };
}

function AudioDestinationNode(audioContext) {
    AudioNode.call(this, audioContext);
    this.nativeAudioNode = this.nativeDestinationNode = holographic.nativeInterface.audioContext.getDestination(audioContext.nativeContext);
}

function SoundBuffer(nativeSoundBuffer) {
    this.nativeSoundBuffer = nativeSoundBuffer;
}

function AudioScheduledSourceNode() {
    
}

function AudioBufferSourceNode(audioContext) {
    AudioNode.call(this, audioContext);
    AudioScheduledSourceNode.call(this);
    this.nativeAudioNode = this.nativeAudioBufferSourceNode = holographic.nativeInterface.audioContext.createBufferSource(audioContext.nativeContext);

    Object.defineProperty(this, 'buffer', {
        get: function () {
            return this._buffer;
        },
        set: function (value) {
            this._buffer = value;
            holographic.nativeInterface.audioBufferSourceNode.setBuffer(this.nativeAudioBufferSourceNode, value.nativeSoundBuffer);
        }
    });

    this.start = function (when) {
        holographic.nativeInterface.audioBufferSourceNode.start(this.nativeAudioBufferSourceNode, when);
    }

    this.stop = function (when) {
        holographic.nativeInterface.audioBufferSourceNode.stop(this.nativeAudioBufferSourceNode, when);
    }

    this.playbackRate = new AudioParam(holographic.nativeInterface.audioBufferSourceNode.getPlaybackRate(this.nativeAudioBufferSourceNode));
}

function AudioParam(nativeAudioParam) {
    this.nativeAudioParam = nativeAudioParam;

    this.setValueAtTime = function (value, startTime) {
        holographic.nativeInterface.audioParam.setValueAtTime(this.nativeAudioParam, value, startTime);
    }
}