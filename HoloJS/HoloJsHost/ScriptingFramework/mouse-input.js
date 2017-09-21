if (typeof holographic === 'undefined') {
    throw new Error("HoloJS missing");
}

holographic.input.mouse = {};
holographic.input.mouse.id = 1;

holographic.input.mouse.events = ['mouseup', 'mousedown', 'mousemove', 'mousewheel'];

holographic.input.mouse.dispatch = function(x, y, button, action) {
    if (!holographic.canvas) {
        return;
    }

    var realButton = button;
    // Native button parameter is actually MouseEvent.buttons. Convert it to MouseEvent.button now
    if (button & 1) {
        realButton = 0;
    } else if (button & 2) {
        realButton = 2;
    } else if (button & 4) {
        realButton = 1;
    }

    holographic.canvas.dispatchMouseFromWindow(x, y, realButton, holographic.input.mouse.events[action]);
    holographic.canvas.ownerDocument.dispatchMouseFromWindow(x, y, realButton, holographic.input.mouse.events[action]);
}