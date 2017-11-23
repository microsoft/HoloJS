(function (global) {
    const SHORTHAND_HEX_REGEX = /^#?([a-f\d])([a-f\d])([a-f\d])$/i;
    const HEX_REGEX = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i;
    const RGBA_REGEX = /^rgba?\((\d+),\s*(\d+),\s*(\d+)(?:,\s*(\d+(?:\.\d+)?))?\)$/;

    function ImageData(buffer, width, height) {
        this.data = buffer;
        this.height = height;
        this.width = width;
    }

    function parseColor(color) {
        if (color.charAt(0) === '#') {
            let hex = color.replace(SHORTHAND_HEX_REGEX, (m, r, g, b) => {
                return r + r + g + g + b + b;
            });

            color = HEX_REGEX.exec(hex);
            return {
                r: parseInt(color[1], 16),
                g: parseInt(color[2], 16),
                b: parseInt(color[3], 16),
                a: 1
            };
        } else if (RGBA_REGEX.test(color)) {
            color = color.match(RGBA_REGEX);
            return {
                r: parseInt(color[1]),
                g: parseInt(color[2]),
                b: parseInt(color[3]),
                a: parseFloat(color[4] || 1)
            };
        } else {
            throw new Error('Canvas colors must either be a hex string or an rgb value.');
        }
    }

    function parseFont(fontString) {
        let fontSize = parseInt(fontString.match(/(\d+)px/)[1]);
        let fontFamily = fontString.match(/ ([^ ]+)$/)[1];

        if (!fontSize || !fontFamily) {
            throw new Error('Fonts must be defined in the format \'<Number>px <Family>\'');
        }

        return {
            size: fontSize,
            family: fontFamily
        }
    }

    function CanvasGradient(x1, y1, x2, y2) {
        this.start = { x: x1, y: y1 };
        this.end = { x: x2, y: y2 };
        this.stops = [];
    }

    CanvasGradient.prototype.addColorStop = function (position, color) {
        color = parseColor(color);
        this.stops.push({
            position,
            color
        });
    };

    holographic.nativeInterface.makeRenderingContext = function() {
        this.ctxNative = holographic.nativeInterface.canvas2d.createContext2D();

        this.drawImage = function (image) {
            let sx = 0,
                sy = 0,
                sWidth = image.width,
                sHeight = image.height,
                dx = 0,
                dy = 0,
                dWidth = image.width,
                dHeight = image.height;

            if (arguments.length === 9) {
                sx = arguments[1];
                sy = arguments[2];
                sWidth = arguments[3];
                sHeight = arguments[4];
                dx = arguments[5];
                dy = arguments[6];
                dWidth = arguments[7];
                dHeight = arguments[8];
            } else if (arguments.length === 3) {
                dx = arguments[1];
                dy = arguments[2];
            } else if (arguments.length === 5) {
                dx = arguments[1];
                dy = arguments[2];
                dWidth = arguments[3];
                dHeight = arguments[4];
            } else {
                throw new Error('Canvas: Invalid number of arguments.');
            }

            holographic.nativeInterface.canvas2d.drawImage(
                this.ctxNative,
                image.getData(),
                { x: sx, y: sy, width: sWidth, height: sHeight },
                { x: dx, y: dy, width: dWidth, height: dHeight }
            );
        }.bind(this);

        this.font = '10px Arial';
        this.fillStyle = 'rgb(0, 0, 0)';

        this.fillText = (text, x, y) => {
            let font = parseFont(this.font);
            let color = parseColor(this.fillStyle);
            let point = { x, y };
            holographic.nativeInterface.canvas2d.fillText(this.ctxNative, text, point, color, font.size, font.family);
        };

        this.clearRect = (x, y, width, height) => {
            holographic.nativeInterface.canvas2d.clearRect(this.ctxNative, { x, y, width, height });
        };

        this.createLinearGradient = function (x1, y1, x2, y2) {
            return new CanvasGradient(x1, y1, x2, y2);
        };

        this.fillRect = (x, y, width, height) => {
            let rect = { x, y, width, height };

            if (this.fillStyle instanceof CanvasGradient) {
                holographic.nativeInterface.canvas2d.fillRectGradient(this.ctxNative, rect, this.fillStyle);
            } else {
                let color = parseColor(this.fillStyle);
                holographic.nativeInterface.canvas2d.fillRect(this.ctxNative, rect, color);
            }
        };

        this.getImageData = (x, y, width, height) => {
            let buffer = holographic.nativeInterface.canvas2d.getImageData(this.ctxNative, { x, y, width, height });
            let imageData = new ImageData(buffer, width, height);
            return imageData;
        };
    }

    // Global variable exports
    global.ImageData = ImageData;
    global.CanvasGradient = CanvasGradient;
})(this);
