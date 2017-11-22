let isHoloJs = (typeof holographic !== 'undefined');

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

let ambientLight = new THREE.AmbientLight(0xFFFFFF, 0.8);
let directionalLight = new THREE.DirectionalLight(0xFFFFFF, 0.5);
let pointLight = new THREE.PointLight(0xFFFFFF, 0.5);

renderer.setSize(window.innerWidth, window.innerHeight);

directionalLight.position.set(0, 2, 0);

scene.add(ambientLight);
scene.add(directionalLight);
scene.add(pointLight);

scene.add(camera);

var onProgress = function (xhr) {
    
};

var onError = function (xhr) {

};

var mtlLoader = new THREE.MTLLoader();
mtlLoader.setCrossOrigin('anonymous');
mtlLoader.load('couchPoofyPillows.mtl', function (materials) {
    materials.preload();
    var objLoader = new THREE.OBJLoader();
    objLoader.setMaterials(materials);
    objLoader.load('couchPoofyPillows.obj', function (result) {
        result.position.set(-10, 0, -10);
        //result.rotateY(Math.PI);
        scene.add(result);
    }, onProgress, onError);
});

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