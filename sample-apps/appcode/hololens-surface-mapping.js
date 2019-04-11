const isHoloJs = typeof navigator.holojs !== 'undefined';

if (isHoloJs === false) {
    throw "spatial mapping not supported in browser";
}

const canvas = document.createElement('canvasvr');

var camera, scene, renderer;

// The surface observer
var surfaceMapper;

// Meshes currently being rendered
var activeMeshes = [];

// Queued mesh changes; to maintain framerate, we only update 1 mesh per render pass
var queuedMeshes = [];

// Group meshes under a group to translate them together in space
var meshesGroup;
var meshMaterial;

init();
animate();

// Cretes a ThreeJS Mesh object from a mesh buffer
function createMeshObjectBuffered(surface) {
    var geometry = new THREE.BufferGeometry();

    // Use the buffers we received from HoloJs
    geometry.setIndex(new THREE.BufferAttribute(surface.indices, 1));
    geometry.addAttribute('position', new THREE.BufferAttribute(surface.vertices, 3));
    geometry.addAttribute('normal', new THREE.BufferAttribute(surface.normals, 3, true));

    var vertexTransform = new THREE.Matrix4();
    vertexTransform.fromArray(surface.position);
    geometry.applyMatrix(vertexTransform);

    return { id: surface.id, mesh: new THREE.Mesh(geometry, meshMaterial) };
}

// Updates the geometry of a mesh
function updateMeshObjectGeometryBuffered(existingGeometry, surface) {
    // Use the mesh buffers as received from HoloJs
    existingGeometry.setIndex(new THREE.BufferAttribute(surface.indices, 1));
    existingGeometry.addAttribute('position', new THREE.BufferAttribute(surface.vertices, 3));
    existingGeometry.addAttribute('normal', new THREE.BufferAttribute(surface.normals, 3, true));

    var vertexTransform = new THREE.Matrix4();
    vertexTransform.fromArray(surface.position);
    existingGeometry.applyMatrix(vertexTransform);
}

function init() {
    scene = new THREE.Scene();
    camera = new THREE.PerspectiveCamera(70, window.innerWidth / window.innerHeight, 0.01, 50);

    scene.add(new THREE.GridHelper(10, 10, 0x444444, 0x444444));

    meshMaterial = new THREE.MeshLambertMaterial({ color : 0x8888bb});

    let hemiLight = new THREE.HemisphereLight(0x888877, 0x777788);
    hemiLight.position.set(0, 0, 0);
    scene.add(hemiLight);

    let pointLight = new THREE.PointLight(0xFFFFFF, 1, 0);
    pointLight.position.set(0, 0, 0);
    scene.add(pointLight);

    renderer = new THREE.WebGLRenderer({canvas, antialias: true });
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.setSize(window.innerWidth, window.innerHeight);

    renderer.vr.enabled = true;

    // Put SR meshes in this group; since ThreeJS shifts the origin 1.6m up for VR without a scene,
    // shift the meshes down 1.6m to have them properly world aligned
    meshesGroup = new THREE.Group();
    meshesGroup.position.set(0, 1.6, 0);
    scene.add(meshesGroup);

    window.addEventListener('resize', onWindowResize, false);

    // Initialize the sufrace mapper, if available
    if (SurfaceMapper.isAvailable()) {
        surfaceMapper = new SurfaceMapper();

        // set callbacks for new, updated and deleted meshes
        surfaceMapper.onnewmesh = onNewMesh;
        surfaceMapper.onupdatedmesh = OnMeshUpdated;
        surfaceMapper.ondeletedmesh = OnMeshDeleted;

        // Set triangle density per m3.
        surfaceMapper.triangleDensity = 1000;

        // Set the bounding cube to scan
        surfaceMapper.boundingCube = {x : 0, y : 0, z : 0, extentX : 2, extentY : 2, extentZ : 2};

        // Start the mapper; when updates are no longer needed, call stop to free device resources
        surfaceMapper.start();
    }
}

// Remove queued mesh if a new update is received or it was deleted
function removeQueuedMesh(meshid) {
    for (let i = 0; i < queuedMeshes.length; i++) {
        if (queuedMeshes[i].id === meshid) {
            queuedMeshes.splice(i, 1);
            console.log('removed queued meshid\r\n');
            break;
        }
    }
}

// Remove active mesh when it gets deleted
function removeActiveMesh(meshid) {
    // Remove mesh if found in active meshes array
    for (let i = 0; i < activeMeshes.length; i++) {
        if (activeMeshes[i].id === meshid) {
             let meshToRemove = activeMeshes.splice(i, 1)[0];
            meshesGroup.remove(meshToRemove.mesh);
            console.log('removed active mesh ' + meshToRemove.id + '\r\n');
            meshToRemove.mesh.geometry.dispose();
            break;
        }
    }
}

function onNewMesh(mesh) {
    console.log('new mesh\r\n');
    queuedMeshes.push(mesh);
}

function OnMeshUpdated(mesh) {
    console.log('updated mesh ' + mesh.id + '\r\n');
    removeQueuedMesh(mesh);
    queuedMeshes.push(mesh);
}

function OnMeshDeleted(meshid) {
    console.log('deleted mesh' + meshid + '\r\n');
    removeActiveMesh(meshid);
    removeQueuedMesh(meshid);
}

function onWindowResize() {

    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();

    renderer.setSize(window.innerWidth, window.innerHeight);
}


function animate() {
    renderer.setAnimationLoop(render);

    navigator.getVRDisplays().then(
        function (value) {
            if (value.length > 0) {
                renderer.vr.enabled = true;
                renderer.vr.setDevice(value[0]);
                value[0].requestPresent([{ source: renderer.domElement }]);
            }
        });
}

// Process at most 1 queued mesh update to maintain framerate
function processQueuedMesh() {
    if (queuedMeshes.length > 0) {
        let meshid = queuedMeshes[0].id;
        let meshUpdated = false;
        // Update the geometry in place if it's already active
        for (let i = 0; i < activeMeshes.length; i++) {
            if (activeMeshes[i].id === meshid) {
                updateMeshObjectGeometryBuffered(activeMeshes[i].mesh.geometry, queuedMeshes[0]);
                console.log('updated active mesh ' + activeMeshes[i].id + '\r\n');
                meshUpdated = true;
                break;
            }
        }

        if (meshUpdated === false) {
            // It's a new mesh. Create a new buffered geometry for it
            console.log('created new mesh ' + queuedMeshes[0].id + '\r\n');
            let mesh3DObject = createMeshObjectBuffered(queuedMeshes[0]);
            meshesGroup.add(mesh3DObject.mesh);
            activeMeshes.push(mesh3DObject);
        }

        queuedMeshes.splice(0, 1);
    }
}

function render() {
    // Process at most 1 queued mesh update to maintain framerate
    processQueuedMesh();
    renderer.render(scene, camera);
}
