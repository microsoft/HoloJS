var global = {};
global.canvas = document.createElement("canvas3D");
global.engine = new BABYLON.Engine(global.canvas, true);

// setups normal camera
var setupNormalCamera = function () {
    if (window.experimentalHolographic) {
        global.camera = new BABYLON.HolographicCamera("Camera", BABYLON.Vector3.Zero(), global.scene);
        global.scene.clearColor = new BABYLON.Color4(0, 0, 0, 0);
    } else {
        // This creates and positions the arc camera
        global.camera = new BABYLON.ArcRotateCamera("ArcRotateCamera", 0, Math.PI / 2, 10, new BABYLON.Vector3(0, 0, 0), global.scene);

        // limiting the upper and lower bounds removes the scrollwheel behaviour
        global.camera.lowerRadiusLimit = global.camera.radius;
        global.camera.upperRadiusLimit = global.camera.radius;

        // This attaches the camera to the canvas
        global.camera.attachControl(global.canvas, false);
    }
}

// setup photosphere
var setupPhotoSphere = function () {

    // Our built-in 'sphere' shape. Params: name, subdivs, size, scene
    var sphere = BABYLON.Mesh.CreateSphere("sphere1", 32, 18, global.scene);

    // load material
    var texture = new BABYLON.Texture("town360high.jpg", global.scene);

    // rotate texture
    texture.wAng = Math.PI;

    // create new material
    var materialPhotoSphere = new BABYLON.StandardMaterial("texturePhotoSphere", global.scene);
    materialPhotoSphere.diffuseTexture = texture;

    // enable backface culling
    materialPhotoSphere.backFaceCulling = false;
    //sphere.scaling.x = -1;

    // set emissive color
    materialPhotoSphere.emissiveColor = new BABYLON.Color3(1.0, 1.0, 1.0);

    // asign material to sphere
    sphere.material = materialPhotoSphere;
}

// Init babylon
var setupBabylon = function () {
    // This creates a basic Babylon Scene object (non-mesh)
    global.scene = new BABYLON.Scene(global.engine);

    // setup camera
    setupNormalCamera();

    // setup photosphere
    setupPhotoSphere();

    // Once the scene is loaded, just register a render loop to render it
    global.engine.runRenderLoop(function () {
        if (window.experimentalHolographic) {
            var viewMatrix = BABYLON.Matrix.FromArray(window.getViewMatrix());
            global.scene.activeCamera.setViewMatrix(viewMatrix);
        }

        global.scene.render();
    });
}

setupBabylon();

