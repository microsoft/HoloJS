var camera, scene, renderer;

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

    let geometry = new THREE.BoxBufferGeometry(0.3, 0.3, 0.3);
    let material = new THREE.MeshBasicMaterial({ color : 0xff0000 });
    let cube = new THREE.Mesh(geometry, material);
    cube.position.z = -1.5;
    cube.position.y = 1.6;
    scene.add(cube);

    // Enter immersive mode if available
    navigator.getVRDisplays().then(
        function (value) {
            if (value.length > 0) {
                renderer.vr.enabled = true;
                renderer.vr.setDevice(value[0]);
                value[0].requestPresent([{ source: renderer.domElement }]);
            }
        });
}

function onWindowResize() {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
}

function render() {
    renderer.render(scene, camera);
}

init();