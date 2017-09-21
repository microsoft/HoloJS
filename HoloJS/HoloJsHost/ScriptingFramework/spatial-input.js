if (typeof holographic === 'undefined') {
    throw new Error("HoloJS missing");
}

holographic.input.spatial = {};

holographic.input.spatial.events = 
    ['sourcepress', 'sourcerelease', 'sourcelost', 'sourcedetected', 'sourceupdate'];

holographic.input.spatial.id = 3;

holographic.input.spatial.dispatch = function (x, y, z, isPressed, sourceKind, type) {
    var spatialInputEvent = {};

    spatialInputEvent.isPressed = isPressed;
    spatialInputEvent.location = { x: x, y: y, z: z };

    spatialInputEvent.preventDefault = function () { };
    spatialInputEvent.srcElement = document.canvas;
    spatialInputEvent.sourceKind = sourceKind;
    spatialInputEvent.type = holographic.input.spatial.events[type];

    // TODO: Implement this
    // holographic.canvas.dispatchSpatialInputFromWindow(spatialInputEvent);
}

