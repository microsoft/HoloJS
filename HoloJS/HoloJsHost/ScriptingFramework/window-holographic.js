if (typeof holographic === 'undefined') {
    throw new Error("HoloJS missing");
}

if (typeof holographic.input === 'undefined') {
    throw new Error("HoloJS input missing");
}

holographic.getViewMatrix = function () {
    return holographic.nativeInterface.window.holographicViewMatrixMid;
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

