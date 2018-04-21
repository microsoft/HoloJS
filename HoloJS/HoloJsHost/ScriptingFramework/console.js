if (typeof holographic === 'undefined') {
    throw new Error("HoloJS missing");
}

var console;

(function () {

    function makeConsole() {
        this.log = function (...entries) {
            holographic.nativeInterface.system.log('log: ' + entries.join(" ") + '\r\n');
        }.bind(this);

        this.warn = function (...entries) {
            holographic.nativeInterface.system.log('warn: ' + entries.join(" ") + '\r\n');
        };

        this.error = function (...entries) {
            holographic.nativeInterface.system.log('error: ' + entries.join(" ") + '\r\n');
        };

        this.info = function (...entries) {
            holographic.nativeInterface.system.log("info: " + entries.join(" ") + "\r\n");
        };

        this.dir = function (...entries) {
            holographic.nativeInterface.system.log("dir: " + entries.join(" ") + "\r\n");
        };

        this.time = function (label) {
            this.log("Start " + label);
        }

        this.timeEnd = function(label) {
            this.log("End " + label);
        }
    }

    console = new makeConsole();
})();

var setTimeout = holographic.nativeInterface.timers.setTimeout;
var clearTimeout = holographic.nativeInterface.timers.clearTimer;
var setInterval = holographic.nativeInterface.timers.setInterval;
var clearInterval = holographic.nativeInterface.timers.clearTimer;