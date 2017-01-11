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
        this._holographicViewMatrix.elements.set(window.getViewMatrix());
        this._holographicViewMatrix.multiply(this._flipMatrix);
        this._holographicTransformMatrix.getInverse(this._holographicViewMatrix);
        this._holographicTransformMatrix.decompose(this.position, this.quaternion, this.scale);
    }
}

let renderer = new THREE.WebGLRenderer({ canvas: canvas, antialias: true });
let scene = new THREE.Scene();
let camera = window.getViewMatrix ? new HolographicCamera() : new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.01, 1000);
let raycaster = new THREE.Raycaster();
let clock = new THREE.Clock();
let loader = new THREE.TextureLoader();
let material = new THREE.MeshStandardMaterial({ vertexColors: THREE.VertexColors, map: new THREE.DataTexture(new Uint8Array(3).fill(255), 1, 1, THREE.RGBFormat) });

let ambientLight = new THREE.AmbientLight(0xFFFFFF, 0.8);
let directionalLight = new THREE.DirectionalLight(0xFFFFFF, 0.5);
let pointLight = new THREE.PointLight(0xFFFFFF, 0.5);

let cube = new THREE.Mesh(new THREE.BoxBufferGeometry(0.2, 0.2, 0.2), material.clone());
let sphere = new THREE.Mesh(initColors(new THREE.SphereBufferGeometry(0.1, 20, 20)), material.clone());
let cone = new THREE.Mesh(initColors(new THREE.ConeBufferGeometry(0.1, 0.2, 20, 20)), material.clone());
let torus = new THREE.Mesh(initColors(new THREE.TorusKnotBufferGeometry(0.2, 0.02, 100, 100)), material.clone());
let cursor = new THREE.Mesh(initColors(new THREE.RingBufferGeometry(0.001, 0.003, 20, 20)), material.clone());

renderer.setSize(window.innerWidth, window.innerHeight);
loader.setCrossOrigin('anonymous');
material.map.needsUpdate = true;

directionalLight.position.set(0, 1, 1);

cube.position.set(0, 0, -1.5);
cube.geometry.addAttribute('color', new THREE.BufferAttribute(Float32Array.from([
    1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, // right - red
    0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, // left - blue
    0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, // top - green
    1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, // bottom - yellow
    0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, // back - cyan
    1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, // front - purple
]), 3));
loader.load('texture.png', tex => { cube.material.map = tex; start(); }, x => x, err => start());

sphere.position.set(0.4, 0, -1.5);
sphere.material.color.set(0xff0000);
sphere.material.roughness = 0.3;
sphere.material.metalness = 0.2;

cone.position.set(-0.4, 0, -1.5);
cone.material.color.set(0x0000ff);
cone.material.roughness = 0.5;

torus.scale.set(1.5, 1.5, 1.5);
torus.material.color.set(0x00ff00);
torus.material.roughness = 0.5;
torus.material.metalness = 1.0;

cursor.position.set(0, 0, -0.5);
cursor.material.transparent = true;
cursor.material.opacity = 0.5;

scene.add(ambientLight);
scene.add(directionalLight);
scene.add(pointLight);

scene.add(cube);
scene.add(sphere);
scene.add(cone);
scene.add(torus);
camera.add(cursor);
scene.add(camera);

cube.frustumCulled = false;
sphere.frustumCulled = false;
cone.frustumCulled = false;
torus.frustumCulled = false;
cursor.frustumCulled = false;

function initColors (geometry) {
    return geometry.addAttribute('color', new THREE.BufferAttribute(new Float32Array(geometry.attributes.position.array.length).fill(1.0), 3));
}

function update (delta, elapsed) {
    window.requestAnimationFrame(() => update(clock.getDelta(), clock.getElapsedTime()));

    pointLight.position.set(0 + 2.0 * Math.cos(elapsed * 0.5), 0, -1.5 + 2.0 * Math.sin(elapsed * 0.5));
    cube.rotation.y += 0.01;
    sphere.scale.x = sphere.scale.y = sphere.scale.z = Math.abs(Math.cos(elapsed * 0.3)) * 0.6 + 1.0;
    cone.position.y = Math.sin(elapsed * 0.5) * 0.1;
    torus.position.z = -2 - Math.abs(Math.cos(elapsed * 0.2));

    raycaster.ray.origin.setFromMatrixPosition(camera.matrixWorld);
    raycaster.ray.direction.set(0, 0, -1).transformDirection(camera.matrixWorld);
    let intersects = raycaster.intersectObjects(scene.children);
    if (intersects.length > 0) {
        cursor.material.color.set(0xFFFF00);
        cursor.material.opacity = 0.8;
        cursor.scale.set(2, 2, 2);
    }
    else {
        cursor.material.color.set(0xFFFFFF);
        cursor.material.opacity = 0.5;
        cursor.scale.set(1, 1, 1);
    }

    if (camera.update) camera.update();

    renderer.render(scene, camera);
}

function start () {
    update(clock.getDelta(), clock.getElapsedTime());
}