if (typeof holographic === 'undefined') {
    throw new Error("HoloJS missing");
}

holographic.input.spatial = {};

holographic.input.spatial.events = 
    ['sourcepress', 'sourcerelease', 'sourcelost', 'sourcedetected', 'sourceupdate'];

holographic.input.spatial.id = 3;

holographic.input.spatial.dispatch = function (x, y, z, isPressed, sourceKind, type) {
    holographic.canvas.dispatchSpatialInputFromWindow(holographic.input.spatial.events[type], isPressed, x, y, z, sourceKind);
}

