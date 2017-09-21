if (typeof holographic === 'undefined') {
    throw new Error("HoloJS missing");
}

if (typeof holographic.input === 'undefined') {
    throw new Error("HoloJS input missing");
}

holographic.getViewMatrix = function () {
    return holographic.nativeInterface.window.holographicViewMatrixMid;
};

holographic.getProjectionMatrix = function () {
    return holographic.nativeInterface.window.holographicProjectionMatrixMid;
};

holographic.getCameraPositionVector = function () {
    return holographic.nativeInterface.window.holographicCameraPositionMid;
};

holographic.getHolographicCameraParameters = function () {
    return {
        mid: {
            viewMatrix: holographic.nativeInterface.window.holographicViewMatrixMid,
            projectionMatrix: holographic.nativeInterface.window.holographicProjectionMatrixMid,
            cameraPosition: holographic.nativeInterface.window.holographicCameraPositionMid
        },
        left: {
            viewMatrix: holographic.nativeInterface.window.holographicViewMatrixLeft,
            projectionMatrix: holographic.nativeInterface.window.holographicProjectionMatrixLeft,
            cameraPosition: holographic.nativeInterface.window.holographicCameraPositionLeft
        },
        right: {
            viewMatrix: holographic.nativeInterface.window.holographicViewMatrixRight,
            projectionMatrix: holographic.nativeInterface.window.holographicProjectionMatrixRight,
            cameraPosition: holographic.nativeInterface.window.holographicCameraPositionRight
        }
    };
};

window.innerWidth = window.width = holographic.nativeInterface.window.getWidth();
window.innerHeight = window.height = holographic.nativeInterface.window.getHeight();

holographic.callbackFromNative = function (type) {
    if (!type) {
        var capturedCallback = holographic.drawCallback;
        holographic.drawCallback = null;
        if (capturedCallback) {
            capturedCallback();
        }
    } else if (type === holographic.input.resize) {
        window.dispatchEvent(window.nativeEvents.events[type]);
    } else if (type === holographic.input.mouse.id) {
        holographic.input.mouse.dispatch(arguments[1], arguments[2], arguments[3], arguments[4]);
    } else if (type === holographic.input.keyboard.id) {
        holographic.input.keyboard.dispatch(arguments[1], arguments[2]);
    }
};

holographic.nativeInterface.window.setCallback(holographic.callbackFromNative);

holographic.experimental = {};
holographic.experimental.autoStereo = false;

window.requestAnimationFrame = function (callback) {
    holographic.drawCallback = callback;
};

function makePerformance() {
    var start = Date.now();
    this.now = function () {
        return Date.now() - start;
    };
}

performance = new makePerformance();