if (typeof holographic === 'undefined') {
    throw new Error("HoloJS missing");
}

var console;

(function () {

    function makeConsole() {
        this.logEntries = [];
        this.warningEntries = [];
        this.errorEntries = [];
        this.infoEntries = [];
        this.dirEntries = [];

        this.log = function (...entries) {
            this.logEntries.push(...entries);
            holographic.nativeInterface.system.log('log: ' + entries.join(" ") + '\r\n');
        }.bind(this);

        this.warn = function (...entries) {
            this.warningEntries.push(...entries);
            holographic.nativeInterface.system.log('warn: ' + entries.join(" ") + '\r\n');
        };

        this.error = function (...entries) {
            this.errorEntries.push(...entries);
            holographic.nativeInterface.system.log('error: ' + entries.join(" ") + '\r\n');
        };

        this.info = function (...entries) {
            this.infoEntries.push(...entries);
            holographic.nativeInterface.system.log("info: " + entries.join(" ") + "\r\n");
        };

        this.dir = function (...entries) {
            this.dirEntries.push(...entries);
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