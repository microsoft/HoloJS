function XMLHttpRequest() {
    
    this.native = new nativeInterface.xhr.create();

    this.nativeCallback = function (type) {
        if (type === "change") {
            this.readyState = arguments[1];
            this.status = arguments[2];
            this.statusText = arguments[3];
            this.responseType = arguments[4];

            if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
                this.fireHandlersByType("load", { target: this });
            }
        }
    };

    nativeInterface.extendWithEventHandling(this);

    this.stubonreadystatechange = function () {
        if (this.onreadystatechangeEvent) {
            this.onreadystatechangeEvent();
        }
    }

    Object.defineProperty(this, "onreadystatechange", {
        get: function () {
            return this.onreadystatechangeEvent;
        },
        set: function (value) {
            if (!value && this.onreadystatechangeEvent) {
                this.onreadystatechangeEvent = value;
                this.removeEventListener("load", this.stubonreadystatechange);
            } else if (value && !this.onreadystatechangeEvent) {
                this.onreadystatechangeEvent = value;
                this.addEventListener("load", this.stubonreadystatechange);
            } else {
                this.onreadystatechangeEvent = value;
            }
        }
    });

    this.open = function (method, url) {
        this.method = method;
        this.url = url;
    };

    this.send = function () {
        nativeInterface.xhr.send(this.native, this.method, this.url, (this.responseType ? this.responseType : "text"));
    };

    Object.defineProperty(this, "responseText", {
        get: function () {
            return nativeInterface.xhr.getResponseText(this.native);
        }
    });

    Object.defineProperty(this, "response", {
        get: function () {
            return nativeInterface.xhr.getResponse(this.native);
        }
    });
}

(function () {
    XMLHttpRequest.DONE = 4;
    XMLHttpRequest.OPENED = 1;
    XMLHttpRequest.HEADERS_RECEIVED = 2;
})();