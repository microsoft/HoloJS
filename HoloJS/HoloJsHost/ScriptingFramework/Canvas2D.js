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

    function Context2D(canvas) {
        this.canvas = canvas;
        this.ctxNative = nativeInterface.canvas2d.createContext2D();

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

            nativeInterface.canvas2d.drawImage(
                this.ctxNative,
                image.native,
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
            nativeInterface.canvas2d.fillText(this.ctxNative, text, point, color, font.size, font.family);
        };

        this.clearRect = (x, y, width, height) => {
            nativeInterface.canvas2d.clearRect(this.ctxNative, { x, y, width, height });
        };

        this.createLinearGradient = function (x1, y1, x2, y2) {
            return new CanvasGradient(x1, y1, x2, y2);
        };

        this.fillRect = (x, y, width, height) => {
            let rect = { x, y, width, height };

            if (this.fillStyle instanceof CanvasGradient) {
                nativeInterface.canvas2d.fillRectGradient(this.ctxNative, rect, this.fillStyle);
            } else {
                let color = parseColor(this.fillStyle);
                nativeInterface.canvas2d.fillRect(this.ctxNative, rect, color);
            }
        };

        this.getImageData = (x, y, width, height) => {
            let buffer = nativeInterface.canvas2d.getImageData(this.ctxNative, { x, y, width, height });
            let imageData = new ImageData(buffer, width, height);
            return imageData;
        };
    }

    nativeInterface.Canvas2D = function () {
        this.isCanvas2D = true;
        this.getContext = (type) => {
            if (type === '2d') {
                if (!this.context) {
                    this.context = new Context2D(this);
                    this.context.canvas = this;
                }

                return this.context;
            }
        };

        Object.defineProperty(this, 'width', {
            get: function () {
                return nativeInterface.canvas2d.getWidth(this.getContext('2d').ctxNative);
            },

            set: function (value) {
                nativeInterface.canvas2d.setWidth(this.getContext('2d').ctxNative, value);
            }
        });

        Object.defineProperty(this, 'height', {
            get: function () {
                return nativeInterface.canvas2d.getHeight(this.getContext('2d').ctxNative);
            },

            set: function (value) {
                nativeInterface.canvas2d.setHeight(this.getContext('2d').ctxNative, value);
            }
        })
    }

    // Global variable exports
    global.ImageData = ImageData;
    global.CanvasGradient = CanvasGradient;
    global.CanvasRenderingContext2D = Context2D;
    global.HTMLCanvasElement = nativeInterface.Canvas2D;
})(this);