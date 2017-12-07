window.AudioContext = function () {

    this.nativeContext = holographic.nativeInterface.audio.createContext();

    this.createGain = function () {
        return new GainNode();
    };

    this.createPanner = function () {
        return new PannerNode();
    };

    this.listener = new AudioListener();

    this.decodeAudioData = function (data, success, error) {
        holographic.nativeInterface.audio.decodeAudioData(this.nativeContext, data, success, error);
    };

    console.log("Created audio context");
};

function AudioNode() {
    this.connect = function (destination, outputIndex, inputIndex) {
        console.log("audio node connect called");
    };

    this.disconnect = function (destination, output, input) {
        console.log("audio node disconnect called");
    };
}

function GainNode() {
    AudioNode.call(this);
}

function PannerNode() {
    AudioNode.call(this);

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

function AudioListener() {
    AudioNode.call(this);

    this.setPosition = function () {
        console.log("panner set position");
    };

    this.setOrientation = function () {
        console.log("panner set orientation");
    };
}