(function () {
    function ImageData(buffer, width, height) {
        this.data = buffer;
        this.height = height;
        this.width = width;
    }

    function Context2D(canvas) {
        this.canvas = canvas;
        this.context2d = nativeInterface.canvas2d.createContext2D();

        this.drawImage = function (image, sx, sy, sWidth, sHeight, dx, dy, dWidth, dHeight) {
            if (arguments.length == 9) {
                nativeInterface.canvas2d.drawImage3(
                    this.context2d,
                    this.canvas.width,
                    this.canvas.height,
                    image.native,
                    image.width,
                    image.height,
                    sx,
                    sy,
                    sWidth,
                    sHeight,
                    dx,
                    dy,
                    dWidth,
                    dHeight);
            } else if (arguments.length == 3) {
                nativeInterface.canvas2d.drawImage1(
                    this.context2d,
                    this.canvas.width,
                    this.canvas.height,
                    image.native,
                    image.width,
                    image.height,
                    dx,
                    dy);
            } else if (arguments.length == 5) {
                nativeInterface.canvas2d.drawImage2(
                    this.context2d,
                    this.canvas.width,
                    this.canvas.height,
                    image.native,
                    image.width,
                    image.height,
                    dx,
                    dy,
                    dWidth,
                    dHeight);
            }
        }.bind(this);

        this.getImageData = function (sx, sy, sw, sh) {
            var buffer = nativeInterface.canvas2d.getImageData(this.context2d, sx, sy, sw, sh);
            var imageData = new ImageData(buffer, sw, sh);
            return imageData;
        }.bind(this);
    }

    nativeInterface.Canvas2D = function () {
        this.isCanvas2D = true;
        this.getContext = function (type) {
            if (type === "2d") {
                if (!this.context) {
                    this.context = new Context2D(this);
                    this.context.canvas = this;
                }

                return this.context;
            }
        }.bind(this);
    }
})();