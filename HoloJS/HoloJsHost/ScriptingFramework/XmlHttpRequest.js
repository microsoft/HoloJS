function XMLHttpRequest() {
    var self = this;
    this.native = new holographic.nativeInterface.xhr.create();

    this.nativeCallback = function(type) {
        if (type === 'change') {
            self.readyState = arguments[1];
            self.status = arguments[2];
            self.statusText = arguments[3];
            self.responseType = arguments[4];

            if (this.readyState === XMLHttpRequest.DONE) {
                if (self.status >= 200 && self.status <= 205) {
                    self.fireHandlersByType('load', {target: self});
                } else {
                    self.fireHandlersByType('error', {target: self});
                }
            }
        }
    };

    this.eventListeners = [];
    this.addEventListener = function (eventType, eventHandler) {
        if ((self.eventListeners.length === 0) && self.native && self.nativeCallback) {
            holographic.nativeInterface.eventing.setCallback(self.native, self.nativeCallback.bind(self));
        }
        self.eventListeners.push({ type: eventType, handler: eventHandler });
    };

    this.removeEventListener = function (eventType, eventHandler) {
        var index;
        var eventListener;
        for (index = 0; index < self.eventListeners.length; index++) {
            eventListener = self.eventListeners[index];
            if (eventListener.type === eventType && eventListener.handler === eventHandler) {
                self.eventListeners.splice(index, 1);
                break;
            }
        }

        if (self.eventListeners.length === 0 && self.native && self.nativeCallback) {
            holographic.nativeInterface.eventing.removeCallback(self.native);
        }
    };

    this.fireHandlersByType = function (type, args) {
        var index;
        var eventListener;
        for (index = 0; index < self.eventListeners.length; index++) {
            eventListener = self.eventListeners[index];
            if (eventListener.type === type) {
                eventListener.handler.call(this, args);
            }
        }
    };

    this.stubonreadystatechange = function() {
        if (this.onreadystatechangeEvent) {
            this.onreadystatechangeEvent();
        }
    };

    Object.defineProperty(this, 'onreadystatechange', {
        get: function() {
            return this.onreadystatechangeEvent;
        },
        set: function(value) {
            if (!value && this.onreadystatechangeEvent) {
                this.onreadystatechangeEvent = value;
                this.removeEventListener('load', this.stubonreadystatechange);
            } else if (value && !this.onreadystatechangeEvent) {
                this.onreadystatechangeEvent = value;
                this.addEventListener('load', this.stubonreadystatechange);
            } else {
                this.onreadystatechangeEvent = value;
            }
        }
    });

    this.open = function(method, url) {
        this.method = method;
        this.url = url;
    };

    this.setRequestHeader = function(header, value) {
        holographic.nativeInterface.xhr.setHeader(this.native, header, value);
    };

    this.getResponseHeader = function(header) {
        return holographic.nativeInterface.xhr.getHeader(this.native, header);
    };

    this.send = function(payload) {
        holographic.nativeInterface.xhr.send(this.native, this.method, this.url, (this.responseType ? this.responseType : 'text'), payload);
    };

    Object.defineProperty(this, 'responseText', {
        get: function() {
            return holographic.nativeInterface.xhr.getResponseText(this.native);
        }
    });

    Object.defineProperty(this, 'response', {
        get: function() {
            return holographic.nativeInterface.xhr.getResponse(this.native);
        }
    });
}

(function() {
XMLHttpRequest.DONE = 4;
XMLHttpRequest.OPENED = 1;
XMLHttpRequest.HEADERS_RECEIVED = 2;
})();