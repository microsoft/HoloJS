let isHoloJs = typeof holographic !== 'undefined';

let canvas = document.createElement(isHoloJs ? 'exp-holo-canvas' : 'canvas');
if (!isHoloJs) {
    document.body.appendChild(canvas);
    document.body.style.margin = document.body.style.padding = 0;
    canvas.style.width = canvas.style.height = "100%";
}

let renderer = new THREE.WebGLRenderer({ canvas: canvas, antialias: true });
let scene = new THREE.Scene();
let camera = isHoloJs && holographic.renderMode > 0 ? new THREE.HolographicCamera() : new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.01, 1000);
let clock = new THREE.Clock();


renderer.setSize(window.innerWidth, window.innerHeight);
scene.add(camera); // this is required for HolographicCamera to function correctly

var controls;

if (!isHoloJs || holographic.renderMode === 0) {
    camera.position.set(0, 0, 1);
    controls = new THREE.OrbitControls(camera, canvas);
}

function update(delta, elapsed) {
    window.requestAnimationFrame(() => update(clock.getDelta(), clock.getElapsedTime()));

    if (camera.update) camera.update();

    //if (sample.update) sample.update(delta, elapsed);

    renderer.render(scene, camera);
}

function start() {
    update(clock.getDelta(), clock.getElapsedTime());
}

// The samples are usually mutually exclusive; enable them one at a time

//var sample = new ChatWebsocketExample(scene, renderer);
//var sample = new SpatialAnchorsExample(scene, renderer);
//var sample = new ShadowsExample(scene, renderer);
//var sample = new BasicCubeExample(scene, renderer);
//var sample = new CanvasRenderingExample(scene, renderer);
//var sample = new LightsExample(scene, renderer);
//var surfaceReconstructionExample = new SurfaceReconstructionExample(scene, renderer);
var sample = new SoundExample(scene, renderer);

start();
