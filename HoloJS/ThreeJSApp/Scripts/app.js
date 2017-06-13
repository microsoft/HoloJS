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
let camera = window.experimentalHolographic === true ? new HolographicCamera() : new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.01, 1000);
let clock = new THREE.Clock();
let loader = new THREE.TextureLoader();
let material = new THREE.MeshPhongMaterial({ color: 0xFFFFFF });
//let material = new THREE.MeshLambertMaterial({color : 0x55BBBB});

let ambientLight = new THREE.AmbientLight(0xFFFFFF, 0.8);
let directionalLight = new THREE.DirectionalLight(0xFFFFFF, 0.5);
let pointLight = new THREE.PointLight(0xFFFFFF, 0.5);

let cube = new THREE.Mesh(new THREE.BoxBufferGeometry(0.2, 0.2, 0.2), material.clone());
let sphere = new THREE.Mesh(initColors(new THREE.SphereBufferGeometry(0.1, 20, 20)), material.clone());
let cone = new THREE.Mesh(initColors(new THREE.ConeBufferGeometry(0.1, 0.2, 20, 20)), material.clone());
let torus = new THREE.Mesh(initColors(new THREE.TorusKnotBufferGeometry(0.2, 0.02, 100, 100)), material.clone());

renderer.setSize(window.innerWidth, window.innerHeight);
loader.setCrossOrigin('anonymous');

directionalLight.position.set(0, 4, 0);

cube.position.set(0, 0, -1.5);
cube.geometry.addAttribute('color', new THREE.BufferAttribute(Float32Array.from([
    1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, // right - red
    0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, // left - blue
    0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, // top - green
    1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, // bottom - yellow
    0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, // back - cyan
    1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, // front - purple
]), 3));
//loader.load('texture.png', tex => { cube.material.map = tex; start(); }, x => x, err => start());

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

scene.add(ambientLight);
scene.add(directionalLight);
scene.add(pointLight);

//scene.add(cube);
//scene.add(sphere);
//scene.add(cone);
scene.add(torus);
scene.add(camera);

cube.frustumCulled = false;
sphere.frustumCulled = false;
cone.frustumCulled = false;
torus.frustumCulled = false;

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

// Listen to spatial input events (hands)
canvas.addEventListener("sourcepress", onSpatialSourcePress);
canvas.addEventListener("spatialmapping", onSurfaceAvailable);

function onSpatialSourcePress(spatialInputEvent) {
    var spatialMapData = window.requestSpatialMappingData();
}

function SRSurfaceMesh(scene) {
    var self = this;
    this.meshObjects = [];
    this.scene = scene;
    this.lowestPoint = Number.MAX_VALUE;

    self.objFormatMesh = [];

    this.updatePositionRelativeToAttachedFor = function (originData) {
        if (originData.OriginToAttachedFor) {
            var matrix = getMatrixFromRestArray(originData.OriginToAttachedFor);
            matrix.transpose();

            for (var meshIndex in self.meshObjects) {
                self.meshObjects[meshIndex].Mesh.matrix = matrix;
            }
        }
    };

    function hideMeshData() {
        for (var meshIndex in self.meshObjects) {
            self.scene.remove(self.meshObjects[meshIndex].Mesh);
        }
    }

    function showMeshData() {
        for (var meshIndex in self.meshObjects) {
            self.scene.add(self.meshObjects[meshIndex].Mesh);
        }
    }

    this.clearMeshData = function (guid) {
        if (guid != undefined) {
            for (var meshIndex in self.meshObjects) {
                if (self.meshObjects[meshIndex].SurfaceId == guid) {
                    self.scene.remove(self.meshObjects[meshIndex].Mesh);
                    self.meshObjects[meshIndex].Mesh.geometry.dispose();
                    self.meshObjects[meshIndex].Mesh.material.dispose();
                    self.meshObjects.splice(meshIndex, 1);
                    break;
                }
            }
        }
        else {
            self.lowestPoint = Number.MAX_VALUE;
            for (var i in self.meshObjects) {
                self.scene.remove(self.meshObjects[i].Mesh);
                self.meshObjects[i].Mesh.geometry.dispose();
                self.meshObjects[i].Mesh.material.dispose();
            }
            self.meshObjects.splice(0, self.meshObjects.length);
        }

    };

    this.setMeshData = function (surfaceData) {
        //self.clearMeshData(newMeshData.Surface.SurfaceId);
        createMeshObjects(surfaceData);
    }

    function getTotalTriangles() {
        var totalTriangles = 0;
        for (var meshIndex in self.meshObjects) {
            totalTriangles += self.meshObjects[meshIndex].triCount;
        }

        return totalTriangles;
    }

    function createMeshObjects(surface) {
        var geometry = new THREE.Geometry();

        var vertexTransform = new THREE.Matrix4();
        vertexTransform.fromArray(surface.originToSurfaceTransform);
        vertexTransform.transpose();

        var normalTransform = new THREE.Matrix4();
        normalTransform.fromArray(surface.normalTransform);
        normalTransform.transpose();

        var vertexIndex = 0;
        // Iterate over vertices, normals and indices and add them to the new geometry object
        for (vertexIndex = 0; vertexIndex <= surface.vertices.length - 4; vertexIndex += 4) {
            geometry.vertices.push(new THREE.Vector4(
                surface.vertices[vertexIndex] / 32767,
                surface.vertices[vertexIndex + 1] / 32767,
                surface.vertices[vertexIndex + 2] / 32767,
                surface.vertices[vertexIndex + 3] / 32767
            ));
        }

        geometry.applyMatrix(vertexTransform);

        for (var indiceIndex = 0; indiceIndex <= surface.indices.length - 3; indiceIndex += 3) {
            var vertexNormals = [];
            for (var normalIndex = 0; normalIndex < 3; normalIndex++) {
                vertexNormals[normalIndex] = new THREE.Vector4(
                    surface.normals[surface.indices[indiceIndex + normalIndex]],
                    surface.normals[surface.indices[indiceIndex + normalIndex] + 1],
                    surface.normals[surface.indices[indiceIndex + normalIndex] + 2],
                    surface.normals[surface.indices[indiceIndex + normalIndex] + 3]
                );
                vertexNormals[normalIndex].applyMatrix4(normalTransform);
            }
            geometry.faces.push(
                //Data passed back from surface reconstruction uses front CW winding order
                new THREE.Face3(
                    surface.indices[indiceIndex + 2],
                    surface.indices[indiceIndex + 1],
                    surface.indices[indiceIndex],
                    vertexNormals));
        }

        //geometry.computeFaceNormals();
        //geometry.computeVertexNormals();
        self.scene.add(new THREE.Mesh(geometry, material));
    }
}

var meshManager = new SRSurfaceMesh(scene);

var added = false;
function onSurfaceAvailable(surfaceData) {
    //if (added === false) {
        meshManager.setMeshData(surfaceData);
        added = true;
    //}
}

start();

