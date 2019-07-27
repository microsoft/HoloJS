var camera, scene, renderer;

function ControllersStatus() {
    this.connectedCount = 0;
    this.thumbPressed = false;
    this.triggerPressed = false;
    this.triggerValue = 0;
    this.gripPressed = false;
    this.menuPressed = false;
    this.thumbpadTouched = false;
    this.thumbpadPressed = false;
    this.thumbstickAxisX = 0;
    this.thumbstickAxisY = 0;
    this.thumbpadAxisX = 0;
    this.thumbpadAxisY = 0;
}

var controllersStatus = new ControllersStatus();
const controllerColorOn = 0xDB3236;
const controllerColorOff = 0xF4C20D;

function init() {
    let canvas = document.createElement('canvasvr');

    renderer = new THREE.WebGLRenderer({ canvas, antialias: true });
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.setSize(window.innerWidth, window.innerHeight);
    renderer.setAnimationLoop(render);

    window.addEventListener('resize', onWindowResize, false);

    camera = new THREE.PerspectiveCamera(70, window.innerWidth / window.innerHeight, 0.2, 1000);
    camera.position.set(0, 1, 0);

    scene = new THREE.Scene();

    // Enter immersive mode if available
    navigator.getVRDisplays().then(
        function (value) {
            if (value.length > 0) {
                renderer.vr.enabled = true;
                renderer.vr.setDevice(value[0]);
                value[0].requestPresent([{ source: renderer.domElement }]);
            }
        });

    const gazeInput = window.dat.GUIVR.addInputObject(camera);
    scene.add(gazeInput.cursor);

    let mainGui = window.dat.GUIVR.create('Controllers');
    mainGui.add(controllersStatus, 'connectedCount').listen();
    mainGui.add(controllersStatus, 'thumbPressed').listen();
    mainGui.add(controllersStatus, 'triggerPressed').listen();
    mainGui.add(controllersStatus, 'triggerValue').min(0).max(1).step(0.05).listen();
    mainGui.add(controllersStatus, 'gripPressed').listen();
    mainGui.add(controllersStatus, 'menuPressed').listen();

    mainGui.add(controllersStatus, 'thumbpadTouched').listen();
    mainGui.add(controllersStatus, 'thumbpadPressed').listen();

    mainGui.add(controllersStatus, 'thumbpadAxisX').min(-1).max(1).step(0.05).listen();
    mainGui.add(controllersStatus, 'thumbpadAxisY').min(-1).max(1).step(0.05).listen();

    mainGui.add(controllersStatus, 'thumbstickAxisX').min(-1).max(1).step(0.05).listen();
    mainGui.add(controllersStatus, 'thumbstickAxisY').min(-1).max(1).step(0.05).listen();

    mainGui.position.z = -1;
    mainGui.position.y = 1.2;
    scene.add(mainGui);

    //THREE.VRController.verbosity = 0.5;
}

function onWindowResize() {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
}

function render() {
    THREE.VRController.update();
    renderer.render(scene, camera);
}

window.addEventListener('vr controller connected', function (event) {

    var controller = event.detail;
    scene.add(controller);

    // Update controller count
    controllersStatus.connectedCount++;

    controller.standingMatrix = renderer.vr.getStandingMatrix();
    controller.head = window.camera;

    let controllerMaterial = new THREE.MeshBasicMaterial({
        color: controllerColorOff
    });

    let controllerMesh = new THREE.Mesh(
        new THREE.CylinderGeometry(0.005, 0.05, 0.1, 6),
        controllerMaterial
    )

    let handleMesh = new THREE.Mesh(
        new THREE.BoxGeometry(0.03, 0.01, 0.03),
        controllerMaterial
    );

    controllerMesh.rotation.x = -Math.PI / 2;
    handleMesh.position.y = -0.05;
    controllerMesh.add(handleMesh);
    controller.userData.mesh = controllerMesh;//  So we can change the color later.
    controller.add(controllerMesh);

    //  Allow this controller to interact with DAT GUI.
    var guiInputHelper = window.dat.GUIVR.addInputObject(controller)
    scene.add(guiInputHelper);

    //  Handle button events to update the UI
    controller.addEventListener('primary press began', function (event) {
        event.target.userData.mesh.material.color.setHex(controllerColorOn);
        guiInputHelper.pressed(true);
    });

    controller.addEventListener('primary press ended', function (event) {
        event.target.userData.mesh.material.color.setHex(controllerColorOff);
        guiInputHelper.pressed(false);
    });


    controller.addEventListener('thumbstick press began', function (event) {
        controllersStatus.thumbPressed = true;
    });

    controller.addEventListener('thumbstick press ended', function (event) {
        controllersStatus.thumbPressed = false;
    });

    controller.addEventListener('trigger press began', function (event) {
        controllersStatus.triggerPressed = true;
    });

    controller.addEventListener('trigger press ended', function (event) {
        controllersStatus.triggerPressed = false;
    });

    controller.addEventListener('trigger value changed', function (event) {
        controllersStatus.triggerValue = event.value;
    });

    controller.addEventListener('grip press began', function (event) {
        controllersStatus.gripPressed = true;
    });

    controller.addEventListener('grip press ended', function (event) {
        controllersStatus.gripPressed = false;
    });

    controller.addEventListener('menu press began', function (event) {
        controllersStatus.menuPressed = true;
    });

    controller.addEventListener('menu press ended', function (event) {
        controllersStatus.menuPressed = false;
    });

    controller.addEventListener('thumbpad touch began', function (event) {
        controllersStatus.thumbpadTouched = true;
    });

    controller.addEventListener('thumbpad touch ended', function (event) {
        controllersStatus.thumbpadTouched = false;
    });

    controller.addEventListener('thumbpad press began', function (event) {
        controllersStatus.thumbpadPressed = true;
    });

    controller.addEventListener('thumbpad press ended', function (event) {
        controllersStatus.thumbpadPressed = false;
    });

    controller.addEventListener('thumbpad axes changed', function (event) {
        controllersStatus.thumbpadAxisX = event.axes[0];
        controllersStatus.thumbpadAxisY = event.axes[1];
    });

    controller.addEventListener('thumbstick axes changed', function (event) {
        controllersStatus.thumbstickAxisX = event.axes[0];
        controllersStatus.thumbstickAxisY = event.axes[1];
    });

    controller.addEventListener('disconnected', function (event) {
        controller.parent.remove(controller);
        controllersStatus.connectedCount--;
    });
})

init();