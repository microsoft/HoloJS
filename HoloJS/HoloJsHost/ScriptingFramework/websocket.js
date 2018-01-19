function WebSocket(url, protocols) {
    this.url = url;
    this.protocols = protocols;
    this.listeners = { "open" : [], "message" : []};

    //this.nativeWebSocket = holographic.nativeInterface.websocket.create;

    this.close = function (code, reason) {
        console.log("close called");
    };

    this.send = function (data) {
        console.log("send called");
    };

    this.addEventListener = function (type, listener, options) {
        if (type === "open") {
            console.log("Add open listener");
            this.listeners.open.push(listener);
        } else if (type === "message") {
            console.log("Add message listener");
            this.listeners.message.push(listener);
        }
    };

    this.removeEventListener = function(type, listener, options) {
        if (type === "open" && this.listeners.open.includes(listener)) {
            console.log("Remove open listener");
            this.listeners.open.splice(this.listeners.open.indexof(listener), 1);
        } else if (type === "message" && this.listeners.message.includes(listener)) {
            console.log("Remove message listener");
            this.listeners.open.splice(this.listeners.message.indexof(listener), 1);
        }
    };
}