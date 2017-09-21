if (typeof holographic === 'undefined') {
    throw new Error("HoloJS missing");
}

holographic.input.keyboard = {};
holographic.input.keyboard.id = 2;
holographic.input.keyboard.keyboardEvents = ['keydown', 'keyup'];

holographic.input.keyboard.dispatch = function (key, type) {
    var keyEvent = {};
    keyEvent.key = key;
    keyEvent.type = holographic.input.keyboard.keyboardEvents[type];

    // TODO: Implement this
    // holographic.canvas.dispatchKeyFromWindow(keyEvent);
}
