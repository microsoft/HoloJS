var nativeInterface = (nativeInterface ? nativeInterface : {});

(function () {
    nativeInterface.extendWithEventHandling = function (base) {
        base.eventListeners = [];
        base.addEventListener = function (eventType, eventHandler) {
            if ((base.eventListeners.length === 0) && base.native && base.nativeCallback) {
                nativeInterface.eventing.setCallback(base.native, base.nativeCallback.bind(base));
            }
            base.eventListeners.push({ type: eventType, handler: eventHandler });
        };

        base.removeEventListener = function (eventType, eventHandler) {
            var index;
            var eventListener;
            for (index = 0; index < base.eventListeners.length; index++) {
                eventListener = base.eventListeners[index];
                if (eventListener.type === eventType && eventListener.handler === eventHandler) {
                    base.eventListeners.splice(index, 1);
                    break;
                }
            }

            if (base.eventListeners.length === 0) {
                nativeInterface.eventing.removeCallback(base.native);
            }
        };

        base.fireHandlersByType = function (type, args) {
            var index;
            var eventListener;
            for (index = 0; index < base.eventListeners.length; index++) {
                eventListener = base.eventListeners[index];
                if (eventListener.type === type) {
                    eventListener.handler.call(this, args);
                }
            }
        };
    };

    nativeInterface.mapNativePropertiesToScripted = function (scriptedObject, nativeObject, properties) {
        var index;
        for (index = 0; index < properties.length; index++) {
            Object.defineProperty(scriptedObject, properties[index].scriptedName, {
                get: function () {
                    return nativeObject[this];
                }.bind(properties[index].nativeName),

                set: function (value) {
                    nativeObject[this] = value;
                }.bind(properties[index].nativeName)
            });
        }
    };

    nativeInterface.mapNativeFunctionsToScripted = function (scriptedObject, nativeObject, functions) {
        var index;
        for (index = 0; index < functions.length; index++) {
            scriptedObject[functions[index].scriptedName] = function (args) {
                return this.thisObject[this.methodName].apply(this.thisObject, arguments);
            }.bind({ thisObject: nativeObject, methodName: functions[index].nativeName });
        }
    };
})();