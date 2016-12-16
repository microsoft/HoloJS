function Image() {

    this.native = new nativeInterface.image.createImage();

    this.nativeCallback = function (type) {
        if (type === "load" && arguments.length > 2) {
            this.width = arguments[1];
            this.height = arguments[2];
        }
        
        this.fireHandlersByType(type);
    };

    nativeInterface.extendWithEventHandling(this);

    this.stubOnLoad = function () {
        if (this.loadEvent) {
            this.loadEvent();
        }
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

    Object.defineProperty(this, "src", {
        get: function () {
            return this.source;
        },
        set: function (value) {
            this.source = value;
            var isBlob = false;
            for (var index = 0; index < URL.objects.length; index++) {
                if (value === URL.objects[index].key) {
                    nativeInterface.image.setImageSourceFromBlob(this.native, URL.objects[index].data);
                    isBlob = true;
                    break;
                }
            }
            if (!isBlob) {
                nativeInterface.image.setImageSource(this.native, value);
            }
        }
    });
}