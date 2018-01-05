window.AudioContext = function () {

    this.nativeContext = holographic.nativeInterface.audio.createContext();

    this.destination = new AudioDestinationNode(this);

    this.createGain = function () {
        return new GainNode(this);
    };

    this.createPanner = function () {
        return new PannerNode(this);
    };

    this.listener = new AudioListener(this);

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
};

function AudioNode(audioContext) {
    this.connect = function (destination, outputIndex, inputIndex) {
        holographic.nativeInterface.audioNode.connect(this.nativeAudioNode, destination.nativeAudioNode, outputIndex, inputIndex);
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

    this.setPosition = function (x, y, z) {
        holographic.nativeInterface.pannerNode.setPosition(this.nativeAudioNode, x, y, z);
    };

    this.setOrientation = function (x, y, z) {
        holographic.nativeInterface.pannerNode.setOrientation(this.nativeAudioNode, x, y, z);
    };

    this.setVelocity = function (x, y, z) {
        holographic.nativeInterface.pannerNode.setVelocity(this.nativeAudioNode, x, y, z);
    };
}

function AudioListener(audioContext) {
    AudioNode.call(this, audioContext);

    this.setPosition = function (x, y, z) {
        holographic.nativeInterface.audioContext.listener_setPosition(this.context.nativeContext, x, y, z);
    };

    this.setOrientation = function (x, y, z, upX, upY, upZ) {
        holographic.nativeInterface.audioContext.listener_setOrientation(this.context.nativeContext, x, y, z, upX, upY, upZ);
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

    function registered_onended() {
        if (typeof this.onended !== 'undefined') {
            this.onended();
        }
    }

    holographic.nativeInterface.audioBufferSourceNode.register_onended(this.nativeAudioBufferSourceNode, registered_onended.bind(this));

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
    };

    this.stop = function (when) {
        holographic.nativeInterface.audioBufferSourceNode.stop(this.nativeAudioBufferSourceNode, when);
    };

    this.playbackRate = new AudioParam(holographic.nativeInterface.audioBufferSourceNode.getPlaybackRate(this.nativeAudioBufferSourceNode));
}

function AudioParam(nativeAudioParam) {
    this.nativeAudioParam = nativeAudioParam;

    this.setValueAtTime = function (value, startTime) {
        holographic.nativeInterface.audioParam.setValueAtTime(this.nativeAudioParam, value, startTime);
    };
}