let isHoloJs = (typeof holographic !== 'undefined');

let canvas = document.createElement(isHoloJs ? 'exp-holo-canvas' : 'canvas');
if (!isHoloJs) {
    document.body.appendChild(canvas);
    document.body.style.margin = document.body.style.padding = 0;
    canvas.style.width = canvas.style.height = "100%";
}

let renderer = new THREE.WebGLRenderer({ canvas: canvas, antialias: true });
let scene = new THREE.Scene();
let camera = (isHoloJs && holographic.renderMode > 0 ? new THREE.HolographicCamera() : new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.01, 1000));
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

    renderer.render(scene, camera);
}

function start() {
    update(clock.getDelta(), clock.getElapsedTime());
}

var loader = new THREE.VTKLoader();
var material = new THREE.MeshLambertMaterial({ color: 0xffffff, side: THREE.DoubleSide });

loader.load("liver.vtk", function (geometry) {

    geometry.center();
    geometry.computeVertexNormals();

    var mesh = new THREE.Mesh(geometry, material);
    mesh.position.set(- 0.075, 0.005, 0);
    mesh.scale.multiplyScalar(0.2);
    scene.add(mesh);
});

let directionalLight = new THREE.DirectionalLight(0xFFFFFF, 0.5);
directionalLight.position.set(0, 2, 0);
scene.add(directionalLight);

let ambientLight = new THREE.AmbientLight(0xFFFFFF, 0.8);
scene.add(ambientLight);

this.pointLight = new THREE.PointLight(0xFFFFFF, 0.5);
scene.add(this.pointLight);

start();