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

var camera, scene, renderer;
var mesh;
var controls;

init();

function init() {

    camera = new THREE.PerspectiveCamera(70, window.innerWidth / window.innerHeight, 0.2, 1000);

    scene = new THREE.Scene();

    var texture = new THREE.TextureLoader().load('media/images/crate.png');

    var geometry = new THREE.BoxBufferGeometry(0.3, 0.3, 0.3);
    var material = new THREE.MeshBasicMaterial({ map: texture });

    mesh = new THREE.Mesh(geometry, material);
    mesh.position.z = -1.5;
    mesh.position.y = 1.6;
    scene.add(mesh);

    renderer = new THREE.WebGLRenderer({canvas, antialias: true });
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.setSize(window.innerWidth, window.innerHeight);

    window.addEventListener('resize', onWindowResize, false);

    controls = new THREE.OrbitControls(camera, canvas);    
    controls.target.set(0, 1.6, -1.5);
    controls.update();

    renderer.setAnimationLoop(render);

    if (isHoloJs === false) {
        // When running in a browser, add a button to ask for permission to enter VR mode
        renderer.vr.enabled = true;
        document.body.appendChild(WEBVR.createButton(renderer));
    } else {
        // In HoloJs the script can enter VR mode without user input
        navigator.getVRDisplays().then(
            function (value) {
                if (value.length > 0) {
                    renderer.vr.enabled = true;
                    renderer.vr.setDevice(value[0]);
                    value[0].requestPresent([{ source: renderer.domElement }]);
                }
            });
    }
}

function onWindowResize() {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
}

function render() {
    mesh.rotation.x += 0.005;
    mesh.rotation.y += 0.01;
    controls.update();

    renderer.render(scene, camera);
}