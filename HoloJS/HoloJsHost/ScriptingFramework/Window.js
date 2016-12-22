var window = (window ? window : {});

(function () {

    function makeLocation(href) {
        this.href = href;
    }
    
    window.drawCallback = null;
    window.location = new makeLocation(nativeInterface.window.getBaseUrl());

    window.requestAnimationFrame = function (callback) {
        window.drawCallback = callback;
    };

    function onMouseEvent(x, y, button, action) {
        if (!document.canvas) {
            return;
        }

        var mouseEvent = {};
        mouseEvent.button = button;
        mouseEvent.clientX = x;
        mouseEvent.clientY = y;

        mouseEvent.preventDefault = function () { };
        mouseEvent.srcElement = document.canvas;

        document.canvas.fireHandlersByType(action, mouseEvent);
    }

    function onKeyboardEvent(event) {
    }

    window.devicePixelRatio = 1;

    Object.defineProperty(window, "innerWidth", {
        get: function () {
            return nativeInterface.window.getWidth();
        }
    });

    Object.defineProperty(window, "innerHeight", {
        get: function () {
            return nativeInterface.window.getHeight()
        }
    });

    Object.defineProperty(window, "width", {
        get: function () {
            return nativeInterface.window.getWidth();
        }
    });

    Object.defineProperty(window, "height", {
        get: function () {
            return nativeInterface.window.getHeight()
        }
    });

    window.nativeCallback = function (type) {
        if (!type) {
            var capturedCallback = window.drawCallback;
            window.drawCallback = null;
            if (capturedCallback !== null) {
                capturedCallback();
            }
        } else if (type === "resize") {
            window.fireHandlersByType(type);
        } else if (type === "mouse") {
            onMouseEvent(arguments[1], arguments[2], arguments[3], arguments[4]);
        } else if (type === "keyboard") {
            onKeyboardEvent(arguments[1], arguments[2]);
        }
        else {
            window.fireHandlersByType(type);
        }
    };

    nativeInterface.extendWithEventHandling(window);

    nativeInterface.window.setCallback(window.nativeCallback);

    window.getViewMatrix = function () {
        return nativeInterface.window.holographicViewMatrix;
    };

    window.getCameraPositionVector = function () {
        return nativeInterface.window.holographicCameraPosition;
    };
})();