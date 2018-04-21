function WebSocket(url, protocols) {
    this.url = url;
    this.protocols = protocols;
    this.listeners = { open : [], message : [], close : [], error: []};
    
    if (typeof protocols === "undefined") {
        this.native = holographic.nativeInterface.websocket.create(url);
    } else {
        this.native = holographic.nativeInterface.websocket.create(url, protocols);
    }

    this.nativeCallback = function (type) {
        if (type === "open") {
            if (typeof this.listeners.onopen !== "undefined") {
                this.listeners.onopen();
            }
            this.listeners.open.forEach(function (listener) {
                listener();
            });
        } else if (type === "close") {
            if (typeof this.listeners.onclose !== "undefined") {
                this.listeners.onclose();
            }
            this.listeners.close.forEach(function (listener) {
                listener();
            });
        } else if (type === "message") {
            if (typeof this.listeners.onmessage !== "undefined") {
                this.listeners.onmessage({ data: arguments[1] });
            }
            this.listeners.message.forEach(function (listener) {
                listener();
            });
        } else if (type === "error") {
            if (typeof this.listeners.onerror !== "undefined") {
                this.listeners.onerror();
            }
            this.listeners.error.forEach(function (listener) {
                listener();
            });
        }
    };

    holographic.nativeInterface.eventing.setCallback(this.native, this.nativeCallback.bind(this));

    this.close = function (code, reason) {
        if (arguments.length === 0) {
            this.native = holographic.nativeInterface.websocket.close(this.native);
        } else if (arguments.length === 1) {
            this.native = holographic.nativeInterface.websocket.close(this.native, code);
        } else {
            this.native = holographic.nativeInterface.websocket.close(this.native, code, reason);
        }
    };

    this.send = function (data) {
        holographic.nativeInterface.websocket.send(this.native, data);
    };

    this.addEventListener = function (type, listener, options) {
        if (type === "open") {
            console.log("Add open listener");
            this.listeners.open.push(listener);
        } else if (type === "message") {
            console.log("Add message listener");
            this.listeners.message.push(listener);
        } else if (type === "error") {
            console.log("Add error listener");
            this.listeners.error.push(listener);
        } else if (type === "close") {
            console.log("Add close listener");
            this.listeners.close.push(listener);
        }
    };

    this.removeEventListener = function(type, listener, options) {
        if (type === "open" && this.listeners.open.includes(listener)) {
            console.log("Remove open listener");
            this.listeners.open.splice(this.listeners.open.indexof(listener), 1);
        } else if (type === "message" && this.listeners.message.includes(listener)) {
            console.log("Remove message listener");
            this.listeners.message.splice(this.listeners.message.indexof(listener), 1);
        } else if (type === "error" && this.listeners.error.includes(listener)) {
            console.log("Remove error listener");
            this.listeners.error.splice(this.listeners.error.indexof(listener), 1);
        } else if (type === "close" && this.listeners.close.includes(listener)) {
            console.log("Remove close listener");
            this.listeners.close.splice(this.listeners.close.indexof(listener), 1);
        }
    };

    Object.defineProperty(this, 'onopen', {
        get: function () {
            return this.listeners.onopen;
        },
        set: function (value) {
            this.listeners.onopen = value;
        }
    });

    Object.defineProperty(this, 'onmessage', {
        get: function () {
            return this.listeners.onmessage;
        },
        set: function (value) {
            this.listeners.onmessage = value;
        }
    });

    Object.defineProperty(this, 'onclose', {
        get: function () {
            return this.listeners.onclose;
        },
        set: function (value) {
            this.listeners.onclose = value;
        }
    });

    Object.defineProperty(this, 'onerror', {
        get: function () {
            return this.listeners.onerror;
        },
        set: function (value) {
            this.listeners.onerror = value;
        }
    });

    Object.defineProperty(this, 'binaryType', {
        get: function () {
            return this._binaryType;
        },
        set: function (value) {
            this._binaryType = value;
            holographic.nativeInterface.websocket.setBinaryType(this.native, value);
        }
    });
}