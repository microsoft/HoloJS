if (typeof holographic === 'undefined') {
    throw new Error("HoloJS missing");
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