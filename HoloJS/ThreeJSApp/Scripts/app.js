let isHoloJs = (typeof holographic !== 'undefined');

let canvas = document.createElement(isHoloJs ? 'exp-holo-canvas' : 'canvas');
if (!isHoloJs) {
    document.body.appendChild(canvas);
    document.body.style.margin = document.body.style.padding = 0;
    canvas.style.width = canvas.style.height = "100%";
}

let renderer = new THREE.WebGLRenderer({ canvas: canvas, antialias: true });
let scene = new THREE.Scene();

let camera = new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.01, 1000);
scene.add(camera);

let clock = new THREE.Clock();

renderer.setSize(window.innerWidth, window.innerHeight);

var controls;

// Not running in HoloJS or rendering flat
if (!isHoloJs || holographic.renderMode === 0) {
    camera.position.set(0, 0, 1);
    controls = new THREE.OrbitControls(camera, canvas);
}

function update(delta, elapsed) {
    window.requestAnimationFrame(() => update(clock.getDelta(), clock.getElapsedTime()));

    if (sample.update) sample.update(delta, elapsed);

    renderer.render(scene, camera);
}

function start() {
    update(clock.getDelta(), clock.getElapsedTime());
}

window.voiceCommands = ["start", "stop"];
window.addEventListener("voicecommand", onVoiceCommand);


function onVoiceCommand(voiceEvent) {
    console.log("Voice command: " + voiceEvent.command + "; confidence: " + voiceEvent.confidence);
}

renderer.vr.enabled = true;
if (isHoloJs) {
    holographic.getVRDisplays().then(function (value) { renderer.vr.setDevice(value[0]); });
} else {
    navigator1.getVRDisplays().then(function (value) { renderer.vr.setDevice(value[0]); });
}

// The samples are usually mutually exclusive; enable them one at a time

var sample = new ShadowsExample(scene, renderer);
//var sample = new BasicCubeExample(scene, renderer);
//var sample = new CanvasRenderingExample(scene, renderer);
//var sample = new LightsExample(scene, renderer);
//var surfaceReconstructionExample = new SurfaceReconstructionExample(scene, renderer);

start();