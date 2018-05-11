(function (global) {
    const SHORTHAND_HEX_REGEX = /^#?([a-f\d])([a-f\d])([a-f\d])$/i;
    const HEX_REGEX = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i;
    const RGBA_REGEX = /^rgba?\((\d+),\s*(\d+),\s*(\d+)(?:,\s*(\d+(?:\.\d+)?))?\)$/;

    // Log not implemented warning. With an option to throw an exception if it's critical.
    function warnNotImplemented(methodName, action) {
        if (action == "throw") {
            console.log("Canvas method " + methodName + " is not implemented.");
            throw new Error("Canvas method " + methodName + " is not implemented.");
        } else {
            console.log("Canvas method " + methodName + " is not implemented, noop.");
        }
    }

    function ImageData(buffer, width, height) {
        this.data = buffer;
        this.height = height;
        this.width = width;
    }

    // CSS color names to color values
    const COLOR_NAMES = {
        aliceblue: '#f0f8ff',
        antiquewhite: '#faebd7',
        aqua: '#00ffff',
        aquamarine: '#7fffd4',
        azure: '#f0ffff',
        beige: '#f5f5dc',
        bisque: '#ffe4c4',
        black: '#000000',
        blanchedalmond: '#ffebcd',
        blue: '#0000ff',
        blueviolet: '#8a2be2',
        brown: '#a52a2a',
        burlywood: '#deb887',
        cadetblue: '#5f9ea0',
        chartreuse: '#7fff00',
        chocolate: '#d2691e',
        coral: '#ff7f50',
        cornflower: '#6495ed',
        cornflowerblue: '#6495ed',
        cornsilk: '#fff8dc',
        crimson: '#dc143c',
        cyan: '#00ffff',
        darkblue: '#00008b',
        darkcyan: '#008b8b',
        darkgoldenrod: '#b8860b',
        darkgray: '#a9a9a9',
        darkgreen: '#006400',
        darkgrey: '#a9a9a9',
        darkkhaki: '#bdb76b',
        darkmagenta: '#8b008b',
        darkolivegreen: '#556b2f',
        darkorange: '#ff8c00',
        darkorchid: '#9932cc',
        darkred: '#8b0000',
        darksalmon: '#e9967a',
        darkseagreen: '#8fbc8f',
        darkslateblue: '#483d8b',
        darkslategray: '#2f4f4f',
        darkslategrey: '#2f4f4f',
        darkturquoise: '#00ced1',
        darkviolet: '#9400d3',
        deeppink: '#ff1493',
        deepskyblue: '#00bfff',
        dimgray: '#696969',
        dimgrey: '#696969',
        dodgerblue: '#1e90ff',
        firebrick: '#b22222',
        floralwhite: '#fffaf0',
        forestgreen: '#228b22',
        fuchsia: '#ff00ff',
        gainsboro: '#dcdcdc',
        ghostwhite: '#f8f8ff',
        gold: '#ffd700',
        goldenrod: '#daa520',
        gray: '#808080',
        green: '#008000',
        greenyellow: '#adff2f',
        grey: '#808080',
        honeydew: '#f0fff0',
        hotpink: '#ff69b4',
        indianred: '#cd5c5c',
        indigo: '#4b0082',
        ivory: '#fffff0',
        khaki: '#f0e68c',
        laserlemon: '#ffff54',
        lavender: '#e6e6fa',
        lavenderblush: '#fff0f5',
        lawngreen: '#7cfc00',
        lemonchiffon: '#fffacd',
        lightblue: '#add8e6',
        lightcoral: '#f08080',
        lightcyan: '#e0ffff',
        lightgoldenrod: '#fafad2',
        lightgoldenrodyellow: '#fafad2',
        lightgray: '#d3d3d3',
        lightgreen: '#90ee90',
        lightgrey: '#d3d3d3',
        lightpink: '#ffb6c1',
        lightsalmon: '#ffa07a',
        lightseagreen: '#20b2aa',
        lightskyblue: '#87cefa',
        lightslategray: '#778899',
        lightslategrey: '#778899',
        lightsteelblue: '#b0c4de',
        lightyellow: '#ffffe0',
        lime: '#00ff00',
        limegreen: '#32cd32',
        linen: '#faf0e6',
        magenta: '#ff00ff',
        maroon: '#800000',
        maroon2: '#7f0000',
        maroon3: '#b03060',
        mediumaquamarine: '#66cdaa',
        mediumblue: '#0000cd',
        mediumorchid: '#ba55d3',
        mediumpurple: '#9370db',
        mediumseagreen: '#3cb371',
        mediumslateblue: '#7b68ee',
        mediumspringgreen: '#00fa9a',
        mediumturquoise: '#48d1cc',
        mediumvioletred: '#c71585',
        midnightblue: '#191970',
        mintcream: '#f5fffa',
        mistyrose: '#ffe4e1',
        moccasin: '#ffe4b5',
        navajowhite: '#ffdead',
        navy: '#000080',
        oldlace: '#fdf5e6',
        olive: '#808000',
        olivedrab: '#6b8e23',
        orange: '#ffa500',
        orangered: '#ff4500',
        orchid: '#da70d6',
        palegoldenrod: '#eee8aa',
        palegreen: '#98fb98',
        paleturquoise: '#afeeee',
        palevioletred: '#db7093',
        papayawhip: '#ffefd5',
        peachpuff: '#ffdab9',
        peru: '#cd853f',
        pink: '#ffc0cb',
        plum: '#dda0dd',
        powderblue: '#b0e0e6',
        purple: '#800080',
        purple2: '#7f007f',
        purple3: '#a020f0',
        rebeccapurple: '#663399',
        red: '#ff0000',
        rosybrown: '#bc8f8f',
        royalblue: '#4169e1',
        saddlebrown: '#8b4513',
        salmon: '#fa8072',
        sandybrown: '#f4a460',
        seagreen: '#2e8b57',
        seashell: '#fff5ee',
        sienna: '#a0522d',
        silver: '#c0c0c0',
        skyblue: '#87ceeb',
        slateblue: '#6a5acd',
        slategray: '#708090',
        slategrey: '#708090',
        snow: '#fffafa',
        springgreen: '#00ff7f',
        steelblue: '#4682b4',
        tan: '#d2b48c',
        teal: '#008080',
        thistle: '#d8bfd8',
        tomato: '#ff6347',
        turquoise: '#40e0d0',
        violet: '#ee82ee',
        wheat: '#f5deb3',
        white: '#ffffff',
        whitesmoke: '#f5f5f5',
        yellow: '#ffff00',
        yellowgreen: '#9acd32'
    };

    function parseColor(color) {
        if (!color) return { r: 0, g: 0, b: 0, a: 0 };
        color = color.toLowerCase();
        if (COLOR_NAMES[color]) color = COLOR_NAMES[color];
        if (color == "transparent" || color == "none") {
            return { r: 0, g: 0, b: 0, a: 0 };
        }
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
            throw new Error('Canvas colors must either be a hex string or an rgb value. (' + color + ')');
        }
    }

    function parseFont(fontString) {
        let sp = fontString.match(/([^"' ]+)|"(?:\\"|[^"])+"|'(?:\\'|[^'])+'/g).map(x => x.replace(/^'([^\']+)'$/, "$1").replace(/^"([^\"]+)"$/, "$1"));
        let fontFamily = "Arial";
        let fontSize = 12;
        let fontStyle = "normal";
        let fontWeight = "normal";
        for (let c of sp) {
            let cl = c.toLowerCase();
            let mPX = c.match(/^([0-9\.]+)px$/);
            if (mPX) {
                fontSize = parseFloat(mPX[1]);
                continue;
            }
            if (cl == "bold") {
                fontWeight = "bold";
                continue;
            }
            if (cl == "italic") {
                fontStyle = "italic";
                continue;
            }
            fontFamily = c;
        }
        return {
            size: fontSize,
            family: fontFamily,
            style: fontStyle,
            weight: fontWeight
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

    holographic.nativeInterface.makeRenderingContext = function () {
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
        this.textAlign = "start";
        this.textBaseline = "bottom";

        this.globalCompositeOperation = "source-over";

        let _globalAlpha = 1;
        Object.defineProperty(this, "globalAlpha", {
            get: () => {
                return _globalAlpha;
            },
            set: (value) => {
                _globalAlpha = value;
                holographic.nativeInterface.canvas2d.setGlobalOpacity(
                    this.ctxNative, value
                );
            }
        })

        this.fillStyle = 'rgb(0, 0, 0)';
        this.strokeStyle = 'rgb(0, 0, 0)';

        this.shadowColor = null;
        this.shadowBlur = 0;
        this.shadowOffsetX = 0;
        this.shadowOffsetY = 0;

        this.lineCap = "butt";
        this.lineJoin = "miter";
        this.lineWidth = 1;
        this.miterLimit = 1;

        this._transform = [1, 0, 0, 1, 0, 0];
        this._stack = [];

        this.fillText = (text, x, y) => {
            let font = parseFont(this.font);
            let color = parseColor(this.fillStyle);
            let point = { x, y };
            let alignment = 0;
            if (this.textAlign == "end" || this.textAlign == "right") alignment = 2;
            if (this.textAlign == "center" || this.textAlign == "middle") alignment = 1;
            holographic.nativeInterface.canvas2d.fillText(this.ctxNative, text, point, color, font, alignment);
        };

        this.strokeText = () => {
            warnNotImplemented("strokeText");
        };

        this.clearRect = (x, y, width, height) => {
            holographic.nativeInterface.canvas2d.clearRect(this.ctxNative, { x, y, width, height });
        };

        this.createLinearGradient = function (x1, y1, x2, y2) {
            return new CanvasGradient(x1, y1, x2, y2);
        };

        this.createPattern = function () {
            warnNotImplemented("createPattern", "throw");
        };

        this.createRadialGradient = function () {
            warnNotImplemented("createRadialGradient", "throw");
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

        this.strokeRect = () => {
            warnNotImplemented("strokeRect");
        };

        this.getImageData = (x, y, width, height) => {
            let buffer = holographic.nativeInterface.canvas2d.getImageData(this.ctxNative, { x, y, width, height });
            let imageData = new ImageData(buffer, width, height);
            return imageData;
        };


        this.setTransform = (a, b, c, d, e, f) => {
            holographic.nativeInterface.canvas2d.setTransform(this.ctxNative, a, b, c, d, e, f);
            this._transform = [a, b, c, d, e, f];
        };

        this.save = () => {
            this._stack.push(JSON.parse(JSON.stringify({
                transform: this._transform
            })));
        };

        this.restore = () => {
            let item = this._stack.pop();
            if (item != null) {
                this.setTransform(item.transform[0], item.transform[1], item.transform[2], item.transform[3], item.transform[4], item.transform[5]);
            }
        };

        this.beginPath = () => {
            holographic.nativeInterface.canvas2d.beginPath(this.ctxNative);
        };

        this.closePath = () => {
            holographic.nativeInterface.canvas2d.closePath(this.ctxNative);
        };

        this.moveTo = (x, y) => {
            holographic.nativeInterface.canvas2d.moveTo(this.ctxNative, { x: x, y: y });
        };

        this.lineTo = (x, y) => {
            holographic.nativeInterface.canvas2d.lineTo(this.ctxNative, { x: x, y: y });
        };

        this.bezierCurveTo = (c1x, c1y, c2x, c2y, x, y) => {
            holographic.nativeInterface.canvas2d.bezierCurveTo(this.ctxNative, { x: c1x, y: c1y }, { x: c2x, y: c2y }, { x: x, y: y });
        };

        this.quadraticCurveTo = (cx, cy, x, y) => {
            holographic.nativeInterface.canvas2d.quadraticCurveTo(this.ctxNative, { x: cx, y: cy }, { x: x, y: y });
        };

        this.rect = (x, y, width, height) => {
            this.moveTo(x, y);
            this.lineTo(x + width, y);
            this.lineTo(x + width, y + height);
            this.lineTo(x, y + height);
            this.closePath();
        };

        this.arc = (cx, cy, r, startAngle, endAngle, counterCW) => {
            holographic.nativeInterface.canvas2d.arc(this.ctxNative, { x: cx, y: cy }, r, startAngle, endAngle, counterCW ? 1 : 0);
        };

        this.arcTo = () => {
            warnNotImplemented("arcTo");
        };

        this.isPointInPath = () => {
            warnNotImplemented("isPointInPath");
            return false;
        };

        this.clip = () => {
            warnNotImplemented("clip");
        };

        this._concatTransform = (a, b, c, d, e, f) => {
            let [a1, b1, c1, d1, e1, f1] = this._transform;
            this.setTransform(
                a * a1 + b * c1,
                a * b1 + b * d1,
                c * a1 + d * c1,
                c * b1 + d * d1,
                e * a1 + f * c1 + e1,
                e * b1 + f * d1 + f1
            );
        };

        this.transform = this._concatTransform;

        this.translate = (dx, dy) => {
            this._concatTransform(
                1, 0, 0, 1, dx,
                dy
            );
        };
        this.scale = (sx, sy) => {
            this._concatTransform(
                sx, 0, 0, sy, 0, 0
            );
        };
        this.rotate = (radians) => {
            this._concatTransform(
                Math.cos(radians), Math.sin(radians),
                -Math.sin(radians), Math.cos(radians),
                0, 0
            );
        };

        this.fill = () => {
            if (this.fillStyle instanceof CanvasGradient) {
                holographic.nativeInterface.canvas2d.fillGradient(this.ctxNative, this.fillStyle);
            } else {
                let color = parseColor(this.fillStyle);
                holographic.nativeInterface.canvas2d.fill(this.ctxNative, color);
            }
        };

        this.stroke = () => {
            let capStyle = 0;
            let joinStyle = 0;
            let lineWidth = this.lineWidth;
            if (this.lineCap == "butt") capStyle = 0;
            if (this.lineCap == "round") capStyle = 1;
            if (this.lineCap == "square") capStyle = 2;
            if (this.lineJoin == "bevel") joinStyle = 1;
            if (this.lineJoin == "miter") joinStyle = 0;
            if (this.lineJoin == "round") joinStyle = 2;
            holographic.nativeInterface.canvas2d.setLineStyle(lineWidth, 0, 0);

            if (this.strokeStyle instanceof CanvasGradient) {
                holographic.nativeInterface.canvas2d.strokeGradient(this.ctxNative, this.strokeStyle);
            } else {
                let color = parseColor(this.strokeStyle);
                holographic.nativeInterface.canvas2d.stroke(this.ctxNative, color);
            }
        };

        this.measureText = (text) => {
            let font = parseFont(this.font);
            let value = holographic.nativeInterface.canvas2d.measureText(this.ctxNative, text, font.size, font.family);
            return {
                width: value
            };
        };

        this.createImageData = () => {
            warnNotImplemented("createImageData", "throw");
        };

        this.putImageData = () => {
            warnNotImplemented("putImageData");
        };
    }

    // Global variable exports
    global.ImageData = ImageData;
    global.CanvasGradient = CanvasGradient;
})(this);
