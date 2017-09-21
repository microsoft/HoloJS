function Image() {
    this.DOM = document.createElement("img");

    this.getData = function () {
        return this.DOM.getData();
    };

    this.addEventListener = function (eventType, eventHandler) {
        this.DOM.addEventListener(eventType, eventHandler);
    };

    this.removeEventListener = function (eventType, eventHandler) {
        this.DOM.removeEventListener(eventType, eventHandler);
    };

    this.stubOnLoad = function() {
        if (this.loadEvent) {
            this.loadEvent();
        }
    };

    Object.defineProperty(this, 'onload', {
        get: function() {
            return this.loadEvent;
        },
        set: function(value) {
            if (!value && this.loadEvent) {
                this.loadEvent = value;
                this.removeEventListener('load', this.stubOnLoad);
            } else if (value && !this.loadEvent) {
                this.loadEvent = value;
                this.addEventListener('load', this.stubOnLoad);
            } else {
                this.loadEvent = value;
            }
        }
    });

    Object.defineProperty(this, 'src', {
        get: function() {
            return this.DOM.src;
        },
        set: function (value) {
            this.DOM.src = value;
        }
    });
}