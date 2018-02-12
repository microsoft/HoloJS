function SpatialAnchorsExample(scene, renderer) {
    var self = this;
    self.isHoloJs = (typeof holographic !== 'undefined');

    self.scene = scene;
    self.renderer = renderer;

    // Create a canvas where we render text
    // Used as a texture for the cube
    self.canvas = document.createElement('canvas'); 
    self.canvas.width = 300;
    self.canvas.height = 300;
    self.ctx = self.canvas.getContext('2d');

    // Draw text
    self.ctx.font = '40px Arial';

    self.texture = new THREE.Texture(self.canvas);
    self.texture.minFilter = THREE.LinearFilter;

    self.cube = new THREE.Mesh(
        new THREE.BoxBufferGeometry(0.4, 0.4, 0.4),
        new THREE.MeshStandardMaterial({
            side: THREE.DoubleSide,
            map: self.texture,
            transparent: true
        })
    );

    function setText(text) {
        // Draw rectangle
        self.ctx.fillStyle = 'rgba(128, 255, 128, 1)';
        self.ctx.fillRect(0, 0, self.canvas.width, self.canvas.height);

        self.ctx.fillStyle = 'rgba(255, 255, 255, 1)';
        self.ctx.fillText(text, 10, 10);
        self.texture.needsUpdate = true;
    }

    setText("Press-hold\nto move cube\nSay \"Save\"\n to persist\n\"Reset\" to clear");

    let ambientLight = new THREE.AmbientLight(0xFFFFFF, 0.8);
    scene.add(ambientLight);

    if (self.isHoloJs) {
        // Load the location where we saved the cube
        holographic.anchors.openAnchor("cubeLocation", function (anchor) {
            if (anchor !== null) {
                self.cubeAnchor = anchor;

                let anchorToOrigin = anchor.transformToOrigin();
                if (anchorToOrigin) {
                    // Decompose the transform to origin to retrieve position and rotation
                    let anchorToOriginMatrix = new THREE.Matrix4();
                    anchorToOriginMatrix.fromArray(anchorToOrigin);

                    let position = new THREE.Vector3();
                    let scale = new THREE.Vector3();
                    let rotation = new THREE.Quaternion();
                    anchorToOriginMatrix.decompose(position, rotation, scale);

                    // Place the cube where the anchor is
                    self.cube.position.set(position.x, position.y, position.z);
                    scene.add(self.cube);
                } else {
                    // The anchor we saved cannot be located anymore; reset the cube to the default location
                    self.cube.position.set(0, 0, 0);
                    scene.add(self.cube);
                }
            } else {
                // There is no saved anchor; place the cube at the default location
                self.cube.position.set(0, 0, 0);
                scene.add(self.cube);
            }
        });

        holographic.anchors.enumerateAnchors(function (anchors) {
            console.log("Enumerated " + anchors.length + " spatial anchors");
        });
    } else {
        // Not running in mixed reality; position the cube at a default location
        self.cube.position.set(0, 0, 0);
        scene.add(self.cube);
    }

    this.update = function (delta, elapsed) {

    };

    // Listen to voice commands;
    // "save" creates an anchor at the current location and saves it to the store
    // "reset" deletes a previously stored anchor and reset the cube to the default location
    window.voiceCommands = ["save", "reset"];
    window.addEventListener("voicecommand", onVoiceCommand);
    function onVoiceCommand(voiceEvent) {
        if (voiceEvent.confidence < 0.5) {
            return;
        }

        if (voiceEvent.command === "save") {
            let cubeAnchor = new SpatialAnchor({ x: self.cube.position.x, y: self.cube.position.y, z: self.cube.position.z });
            holographic.anchors.saveAnchor(cubeAnchor, "cubeLocation", function (result) {
                if (result.success === true) {
                    setText("Anchor saved");
                } else {
                    setText("Failed to save!");
                }
            });
        } else if (voiceEvent.command === "reset") {
            holographic.anchors.deleteAnchor("cubeLocation", function () {
                self.cube.position.set(0, 0, 0);
                setText("Anchor reset!");
            });
        }
    }

    // Listen to hands input; allows moving the cube with hands input
    canvas.addEventListener("sourcepress", onSpatialSourcePress);
    canvas.addEventListener("sourcerelease", onSpatialSourceRelease);
    canvas.addEventListener("sourceupdate", onSpatialSourceUpdate);
    // treat source lost the same way as source release - stop moving the cube when hands input is lost
    canvas.addEventListener("sourcelost", onSpatialSourceRelease);

    self.spatialInputTracking = false;
    self.lastSpatialInputX = 0;
    self.lastSpatialInputY = 0;
    self.lastSpatialInputZ = 0;

    function onSpatialSourcePress(spatialInputEvent) {
        // Remember last hand position
        self.lastSpatialInputX = spatialInputEvent.x;
        self.lastSpatialInputY = spatialInputEvent.y;
        self.lastSpatialInputZ = spatialInputEvent.z;

        self.spatialInputTracking = true;
    }

    function onSpatialSourceRelease(spatialInputEvent) {
        self.spatialInputTracking = false;
    }

    function onSpatialSourceUpdate(spatialInputEvent) {
        if (self.spatialInputTracking === true) {
            // Compute new cube position based on hand delta movement
            self.cube.position.x -= 3 * (self.lastSpatialInputX - spatialInputEvent.x);
            self.cube.position.y -= 3 * (self.lastSpatialInputY - spatialInputEvent.y);
            self.cube.position.z -= 3 * (self.lastSpatialInputZ - spatialInputEvent.z);

            // Remember last hand position
            self.lastSpatialInputX = spatialInputEvent.x;
            self.lastSpatialInputY = spatialInputEvent.y;
            self.lastSpatialInputZ = spatialInputEvent.z;
        }
    }
}