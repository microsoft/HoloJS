var document;
var navigator;
var console;

var setTimeout = nativeInterface.system.setTimeout;

function Source() {

}

function URL() {
    URL.index = 0;
    URL.createObjectURL = function (blob) {
        URL.objects.push({ data: blob, key: URL.index });
        return URL.index++;
    }

    URL.revokeObjectURL = function(url) {
        for (var index = 0; index < URL.objects.length; index++) {
            if (url === URL.objects[index].key) {
                URL.objects.splice(index, 1);
                break;
            }
        }
    }

    URL.objects = [];
}

URL();

function HTMLElement() {

}

function Anchor() {

}

(function () {

    function makeConsole() {
        this.messages = [];
        this.log = function (entry) {
            this.messages.push(entry);
        }.bind(this);

        this.warn = function () {
            this.messages.push(entry);
        }
    }

    function makeDocument() {
        this.createElement = function (type) {
            if (type === "canvas3D") {
                if (!this.canvas) {
                    this.canvas = new makeCanvas3D();
                }
                return this.canvas;
            } else if (type === "canvas") {
                return new nativeInterface.Canvas2D();
            } else if (type === 'a') {
                return new Anchor();
            } else if (type === "img") {
                return new Image();
            } else if (type === "video") {
                return new HTMLVideoElement();
            } else if (type === "source") {
                return new Source();
            } else {
                return null;
            }
        }.bind(this);

        nativeInterface.extendWithEventHandling(this);

        this.getElementById = function (id) {
            return null;
        };

        this.createElementNS = function (ns, type) {
            return this.createElement(type);
        }.bind(this);
    }

    function makeCanvas3D() {
        this.getContext = function (type) {
            if (type === "experimental-webgl" || type === "webgl") {
                if (this.context) {
                    return this.context;
                } else {
                    this.context = new nativeInterface.makeWebGLRenderingContext();
                    return this.context;
                }
            }
        }.bind(this);

        // Lock the size of the canvas3D to the size of the window
        nativeInterface.mapNativePropertiesToScripted(
            this,
            window,
            [{ scriptedName: "width", nativeName: "innerWidth" },
            { scriptedName: "height", nativeName: "innerHeight" },
            { scriptedName: "clientWidth", nativeName: "innerWidth" },
            { scriptedName: "clientHeight", nativeName: "innerHeight" }
            ]);

        nativeInterface.extendWithEventHandling(this);

        this.getBoundingClientRect = function () {
            var rect = {};
            rect.top = 0;
            rect.bottom = window.innerHeight;

            rect.width = window.innerWidth;
            rect.height = window.innerHeight;

            rect.left = 0;
            rect.right = window.innerWidth;

            rect.x = 0;
            rect.y = 0;

            return rect;
        };

        // Stub CSS style
        this.style = {};

        this.releasePointerCapture = function () { };
        this.setPointerCapture = function () { };
    }

    function makeNavigator() {

    }

    document = new makeDocument();
    console = new makeConsole();
    navigator = new makeNavigator();
    console = new makeConsole();
})();