if (typeof holographic === 'undefined') {
    throw new Error("HoloJS missing");
}

holographic.anchors = {};

function SpatialAnchor() {
    if (arguments.length === 1) {
        let position = arguments[0];
        this.external = holographic.nativeInterface.anchors.createAnchor(position.x, position.y, position.z);
    } if (arguments.length === 2) {
        let position = arguments[0];
        let rotation = arguments[1];
        this.external = holographic.nativeInterface.anchors.createAnchor(position.x, position.y, position.z, rotation.x, rotation.y, rotation.z, rotation.w);
    }

    this.transformToOrigin = function () {
        return holographic.nativeInterface.anchors.getTransformToOrigin(this.external);
    };
}

holographic.anchors.enumerateAnchors = function (callback) {
    holographic.nativeInterface.anchors.enumerateAnchors(function (nativeAnchors) {
        if (arguments.length > 0) {
            spatialAnchors = [];
            for (var i = 0; i < nativeAnchors.length; i++) {
                let scriptAnchor = new SpatialAnchor();
                scriptAnchor.external = nativeAnchors[i].anchor;
                spatialAnchors.push({ anchor: scriptAnchor, id: nativeAnchors[i].id });
            }
            callback(spatialAnchors);
        } else {
            callback([]);
        }
    });
};

holographic.anchors.openAnchor = function (name, done) {
    holographic.nativeInterface.anchors.openAnchor(name, function (nativeAnchor) {
        if (arguments.length > 0 && (typeof nativeAnchor !== 'undefined')) {
            var newAnchor = new SpatialAnchor();
            newAnchor.external = nativeAnchor;
            done(newAnchor);
        } else {
            done(null);
        }
    });
};

holographic.anchors.deleteAnchor = function (name, done) {
    holographic.nativeInterface.anchors.deleteAnchor(name, function () {
        done();
    });
};

holographic.anchors.saveAnchor = function (anchor, name, done) {
    holographic.nativeInterface.anchors.saveAnchor(anchor.external, name, function (result) {
        if (arguments.length > 0 && (typeof result !== 'undefined')) {
            done({ success: result });
        } else {
            done({ success: false });
        }
    });
};