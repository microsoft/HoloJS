let isHoloJs = (typeof holographic !== 'undefined');
let canvas = document.createElement(isHoloJs ? 'exp-holo-canvas' : 'canvas');

if (!isHoloJs) {
    document.body.appendChild(canvas);
    document.body.style.margin = document.body.style.padding = 0;
    canvas.style.width = canvas.style.height = "100%";
}

let renderer = new THREE.WebGLRenderer({ canvas: canvas, antialias: true });
let scene = new THREE.Scene();
let camera = (isHoloJs && holographic.renderMode > 0) ? new THREE.HolographicCamera() : new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.01, 1000);
let clock = new THREE.Clock();

let material = new THREE.MeshStandardMaterial({ vertexColors: THREE.VertexColors });

let ambientLight = new THREE.AmbientLight(0xFFFFFF, 0.8);
let directionalLight = new THREE.DirectionalLight(0xFFFFFF, 0.5);
let pointLight = new THREE.PointLight(0xFFFFFF, 0.5);

renderer.setSize(window.innerWidth, window.innerHeight);
directionalLight.position.set(0, 2, 0);

scene.add(ambientLight);
scene.add(directionalLight);
scene.add(pointLight);
scene.add(camera); // this is required for HolographicCamera to function correctly

let fontLoader = new THREE.FontLoader();
fontLoader.load('../threejs/fonts/helvetiker_regular.typeface.json', function (font) {
    var textGeometry = new THREE.TextGeometry("Please load content from a browser", {
        font: font,
        size: 0.2,
        height: 0.05,
        curveSegments: 12
    });

    let textMesh = new THREE.Mesh(textGeometry, material);
    textMesh.position.z = -3;
    textMesh.position.x = -2;
    scene.add(textMesh);
}); //end load function

var controls;

if (!isHoloJs || holographic.renderMode === 0) {
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