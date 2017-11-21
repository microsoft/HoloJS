if (typeof holographic === 'undefined') {
    throw new Error("HoloJS missing");
}

holographic.input.keyboard = {};
holographic.input.keyboard.id = 2;
holographic.input.keyboard.keyboardEvents = ['keydown', 'keyup'];

holographic.input.keyboard.dispatch = function (key, type) {
    holographic.canvas.dispatchKeyboardFromWindow(key, holographic.input.keyboard.keyboardEvents[type]);
}
