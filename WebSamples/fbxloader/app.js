let canvas = document.createElement(window.getViewMatrix ? 'canvas3D' : 'canvas');
if (!window.getViewMatrix) {
    document.body.appendChild(canvas);
    document.body.style.margin = document.body.style.padding = 0;
    canvas.style.width = canvas.style.height = "100%";
}

class HolographicCamera extends THREE.Camera {

    constructor () {
        super();
        this._holographicViewMatrix = new THREE.Matrix4();
        this._holographicTransformMatrix = new THREE.Matrix4();
        this._flipMatrix = new THREE.Matrix4().makeScale(-1, 1, 1);
    }

    update () {
        this._holographicViewMatrix.fromArray(window.getViewMatrix());
        this._holographicViewMatrix.multiply(this._flipMatrix);
        this._holographicTransformMatrix.getInverse(this._holographicViewMatrix);
        this._holographicTransformMatrix.decompose(this.position, this.quaternion, this.scale);
    }
}

let renderer = new THREE.WebGLRenderer({ canvas: canvas, antialias: true });
let scene = new THREE.Scene();
let camera = window.experimentalHolographic === true ? new HolographicCamera() : new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.01, 1000);
let clock = new THREE.Clock();

let ambientLight = new THREE.AmbientLight(0xFFFFFF, 0.8);
let directionalLight = new THREE.DirectionalLight(0xFFFFFF, 0.5);
let pointLight = new THREE.PointLight(0xFFFFFF, 0.5);

renderer.setSize(window.innerWidth, window.innerHeight);

directionalLight.position.set(0, 2, 0);

scene.add(ambientLight);
scene.add(directionalLight);
scene.add(pointLight);

scene.add(camera);

var onProgressModel = function (xhr) {
    console.log("progress");
};

var onErrorModel = function (xhr) {
    console.log("model load error");
};

// FBXLoader expects the Zlib library to be in window? Inflate.min.js does not put it there so do it manually to make FBXLoader happy
window.Zlib = Zlib;

// Load the FBX model.
// After the FBX file is added to the solution, right click on it, goto Properties and make sure that:
//   1. "Item type" is "Does not participate in build"
//   2. Content is "yes"
// Alternatively, host the FBX file online and use the URL here.
var fbxLoader = new THREE.FBXLoader();
var path = 'fbxloader/green.fbx';
fbxLoader.load(path, function (object) {
    object.frustumCulled = false;
    scene.add(object);
}, onProgressModel, onErrorModel);


var controls;

if (window.experimentalHolographic !== true) {
    camera.position.set(0, 0, 1);
    controls = new THREE.OrbitControls(camera, canvas);
}

function initColors (geometry) {
    return geometry.addAttribute('color', new THREE.BufferAttribute(new Float32Array(geometry.attributes.position.array.length).fill(1.0), 3));
}

function update (delta, elapsed) {
    window.requestAnimationFrame(() => update(clock.getDelta(), clock.getElapsedTime()));

    pointLight.position.set(0 + 2.0 * Math.cos(elapsed * 0.5), 0, -1.5 + 2.0 * Math.sin(elapsed * 0.5));

    if (camera.update) camera.update();

    renderer.render(scene, camera);
}

function start () {
    update(clock.getDelta(), clock.getElapsedTime());
}

start();