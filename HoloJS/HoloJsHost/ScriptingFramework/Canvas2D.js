(function() {
function ImageData(buffer, width, height)
{
    this.data = buffer;
    this.height = height;
    this.width = width;
}

function Context2D(canvas)
{
    this.canvas = canvas;
    this.context2d = holographic.nativeInterface.canvas2d.createContext2D();

    this.drawImage = function(image) {
        if (arguments.length == 9) {
            holographic.nativeInterface.canvas2d.drawImage3(this.context2d,
                                                this.canvas.width,
                                                this.canvas.height,
                                                image.native,
                                                image.width,
                                                image.height,
                                                arguments[1],   // sx,
                                                arguments[2],   // sy,
                                                arguments[3],   // sWidth,
                                                arguments[4],   // sHeight,
                                                arguments[5],   // dx,
                                                arguments[6],   // dy,
                                                arguments[7],   // dWidth,
                                                arguments[8]);  // dHeight);
        } else if (arguments.length == 3) {
            holographic.nativeInterface.canvas2d.drawImage1(this.context2d,
                                                this.canvas.width,
                                                this.canvas.height,
                                                image.native,
                                                image.width,
                                                image.height,
                                                arguments[1],   // dx,
                                                arguments[2]);  // dy
        } else if (arguments.length == 5) {
            holographic.nativeInterface.canvas2d.drawImage2(this.context2d,
                                                this.canvas.width,
                                                this.canvas.height,
                                                image.native,
                                                image.width,
                                                image.height,
                                                arguments[1],   // dx,
                                                arguments[2],   // dy
                                                arguments[3],   // dWidth,
                                                arguments[4]);  // dHeight
        }
    }.bind(this);

    this.getImageData = function(sx, sy, sw, sh) {
        var buffer = holographic.nativeInterface.canvas2d.getImageData(this.context2d, sx, sy, sw, sh);
        var imageData = new ImageData(buffer, sw, sh);
        return imageData;
    }.bind(this);
}

holographic.nativeInterface.Canvas2D = function() {
    this.isCanvas2D = true;
    this.getContext = function(type) {
        if (type === '2d') {
            if (!this.context) {
                this.context = new Context2D(this);
                this.context.canvas = this;
            }

            return this.context;
        }
    }.bind(this);
}
})();