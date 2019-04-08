// Check if running in a HoloJs host
const isHoloJs = typeof navigator.holojs !== 'undefined';

// If running in HoloJs, create a VR canvas
const canvas = document.createElement(isHoloJs ? 'canvasvr' : 'canvas');

if (isHoloJs === false) {
    // If running in browser, add the canvas to the DOM
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
    document.body.appendChild(canvas);
}

var engine = new BABYLON.Engine(canvas, true); // Generate the BABYLON 3D engine

var createScene = function () {

    // Create the scene space
    var scene = new BABYLON.Scene(engine);
    var camera = new BABYLON.WebVRFreeCamera("camera1", new BABYLON.Vector3(0, 0, 0), scene);

    if (isHoloJs === false) {
        var vrHelper = scene.createDefaultVRExperience();
        scene.onPointerDown = function () {
            scene.onPointerDown = undefined;
            camera.attachControl(canvas, true);
        };
    }    
    else {
        camera.attachControl(canvas, true);
    }

    // Add lights to the scene
    var light1 = new BABYLON.HemisphericLight("light1", new BABYLON.Vector3(1, 1, 0), scene);
    var light2 = new BABYLON.PointLight("light2", new BABYLON.Vector3(0, 1, -1), scene);

    // Add and manipulate meshes in the scene
    var sphere = BABYLON.MeshBuilder.CreateSphere("sphere", { diameter: 0.3 }, scene);

    return scene;
};

var scene = createScene();

engine.runRenderLoop(function () {
    scene.render();
});

window.addEventListener("resize", function () {
    engine.resize();
});