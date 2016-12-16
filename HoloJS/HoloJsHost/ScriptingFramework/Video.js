function HTMLVideoElement() {

    this.native = new nativeInterface.video.createVideo();

    this.nativeCallback = function (type) {
        if (type === "load" && arguments.length > 2) {
            this.width = arguments[1];
            this.height = arguments[2];
            this.fireHandlersByType('canplaythrough');
        }

        this.fireHandlersByType(type);
    };

    nativeInterface.extendWithEventHandling(this);

    this.stubOnLoad = function () {
        if (this.loadEvent) {
            this.loadEvent();
        }
    }

    this.play = function () {

    }

    Object.defineProperty(this, "onload", {
        get: function () {
            return this.loadEvent;
        },
        set: function (value) {
            if (!value && this.loadEvent) {
                this.loadEvent = value;
                this.removeEventListener("load", this.stubOnLoad);
            } else if (value && !this.loadEvent) {
                this.loadEvent = value;
                this.addEventListener("load", this.stubOnLoad);
            } else {
                this.loadEvent = value;
            }
        }
    });

    this.appendChild = function (source) {
        if (source instanceof Source) {
            this.source = source.src;
            nativeInterface.video.setVideoSource(this.native, source.src);
        }
    }

    Object.defineProperty(this, "src", {
        get: function () {
            return this.source;
        },
        set: function (value) {
            this.source = value;
            nativeInterface.video.setVideoSource(this.native, value);
        }
    });
}