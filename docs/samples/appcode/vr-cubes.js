const isHoloJs = typeof navigator.holojs !== 'undefined';
const canvas = document.createElement(isHoloJs ? 'canvasvr' : 'canvas');

if (isHoloJs === false) {
    // If running in browser, add the canvas to the DOM
    document.body.appendChild(canvas);
}

var clock = new THREE.Clock();

var container;
var camera, scene, raycaster, renderer;

var room;
var isMouseDown = false;

var INTERSECTED;
var crosshair;

init();
animate();

function init() {
    scene = new THREE.Scene();
    scene.background = new THREE.Color(0x505050);

    camera = new THREE.PerspectiveCamera(70, window.innerWidth / window.innerHeight, 0.1, 10);
    scene.add(camera);

    crosshair = new THREE.Mesh(
        new THREE.RingBufferGeometry(0.02, 0.04, 32),
        new THREE.MeshBasicMaterial({
            color: 0xffffff,
            opacity: 0.5,
            transparent: true
        })
    );
    crosshair.position.z = - 2;
    camera.add(crosshair);

    room = new THREE.LineSegments(
        new THREE.BoxLineGeometry(6, 6, 6, 10, 10, 10),
        new THREE.LineBasicMaterial({ color: 0x808080 })
    );
    room.position.y = 3;
    scene.add(room);

    scene.add(new THREE.HemisphereLight(0x606060, 0x404040));

    var light = new THREE.DirectionalLight(0xffffff);
    light.position.set(1, 1, 1).normalize();
    scene.add(light);

    var geometry = new THREE.BoxBufferGeometry(0.15, 0.15, 0.15);

    for (var i = 0; i < 200; i++) {

        var object = new THREE.Mesh(geometry, new THREE.MeshLambertMaterial({ color: Math.random() * 0xffffff }));

        object.position.x = Math.random() * 4 - 2;
        object.position.y = Math.random() * 4 - 2;
        object.position.z = Math.random() * 4 - 2;

        object.rotation.x = Math.random() * 2 * Math.PI;
        object.rotation.y = Math.random() * 2 * Math.PI;
        object.rotation.z = Math.random() * 2 * Math.PI;

        object.scale.x = Math.random() + 0.5;
        object.scale.y = Math.random() + 0.5;
        object.scale.z = Math.random() + 0.5;

        object.userData.velocity = new THREE.Vector3();
        object.userData.velocity.x = Math.random() * 0.01 - 0.005;
        object.userData.velocity.y = Math.random() * 0.01 - 0.005;
        object.userData.velocity.z = Math.random() * 0.01 - 0.005;

        room.add(object);

    }

    raycaster = new THREE.Raycaster();

    renderer = new THREE.WebGLRenderer({ canvas, antialias: true });
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.setSize(window.innerWidth, window.innerHeight);
    renderer.vr.enabled = true;

    renderer.domElement.addEventListener('mousedown', onMouseDown, false);
    renderer.domElement.addEventListener('mouseup', onMouseUp, false);
    renderer.domElement.addEventListener('touchstart', onMouseDown, false);
    renderer.domElement.addEventListener('touchend', onMouseUp, false);

    window.addEventListener('resize', onWindowResize, false);

    //

    window.addEventListener('vrdisplaypointerrestricted', onPointerRestricted, false);
    window.addEventListener('vrdisplaypointerunrestricted', onPointerUnrestricted, false);
}

function onMouseDown() {
    isMouseDown = true;
}

function onMouseUp() {
    isMouseDown = false;
}

function onPointerRestricted() {

    var pointerLockElement = renderer.domElement;
    if (pointerLockElement && typeof (pointerLockElement.requestPointerLock) === 'function') {
        pointerLockElement.requestPointerLock();
    }
}

function onPointerUnrestricted() {

    var currentPointerLockElement = document.pointerLockElement;
    var expectedPointerLockElement = renderer.domElement;
    if (currentPointerLockElement && currentPointerLockElement === expectedPointerLockElement && typeof (document.exitPointerLock) === 'function') {

        document.exitPointerLock();

    }

}

function onWindowResize() {

    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();

    renderer.setSize(window.innerWidth, window.innerHeight);

}

//

function animate() {

    renderer.setAnimationLoop(render);

    if (isHoloJs === false) {
        // When running in a browser, add a button to ask for permission to enter VR mode
        document.body.appendChild(WEBVR.createButton(renderer));
    } else {
        // In HoloJs the script can enter VR mode without user input
        navigator.getVRDisplays().then(
            function (value) {
                if (value.length > 0) {
                    renderer.vr.enabled = true;
                    renderer.vr.setDevice(value[0]);
                    value[0].requestPresent([{ source: renderer.domElement }]);
                }
            });
    }
}

function render() {

    var delta = clock.getDelta() * 60;

    if (isMouseDown === true) {

        var cube = room.children[0];
        room.remove(cube);

        cube.position.set(0, 0, - 0.75);
        cube.position.applyQuaternion(camera.quaternion);
        cube.userData.velocity.x = (Math.random() - 0.5) * 0.02 * delta;
        cube.userData.velocity.y = (Math.random() - 0.5) * 0.02 * delta;
        cube.userData.velocity.z = (Math.random() * 0.01 - 0.05) * delta;
        cube.userData.velocity.applyQuaternion(camera.quaternion);
        room.add(cube);

    }

    // find intersections

    raycaster.setFromCamera({ x: 0, y: 0 }, camera);

    var intersects = raycaster.intersectObjects(room.children);

    if (intersects.length > 0) {

        if (INTERSECTED != intersects[0].object) {

            if (INTERSECTED) INTERSECTED.material.emissive.setHex(INTERSECTED.currentHex);

            INTERSECTED = intersects[0].object;
            INTERSECTED.currentHex = INTERSECTED.material.emissive.getHex();
            INTERSECTED.material.emissive.setHex(0xff0000);

        }

    } else {

        if (INTERSECTED) INTERSECTED.material.emissive.setHex(INTERSECTED.currentHex);

        INTERSECTED = undefined;

    }

    // Keep cubes inside room

    for (var i = 0; i < room.children.length; i++) {

        var cube = room.children[i];

        cube.userData.velocity.multiplyScalar(1 - (0.001 * delta));

        cube.position.add(cube.userData.velocity);

        if (cube.position.x < - 3 || cube.position.x > 3) {

            cube.position.x = THREE.Math.clamp(cube.position.x, - 3, 3);
            cube.userData.velocity.x = - cube.userData.velocity.x;

        }

        if (cube.position.y < - 3 || cube.position.y > 3) {

            cube.position.y = THREE.Math.clamp(cube.position.y, - 3, 3);
            cube.userData.velocity.y = - cube.userData.velocity.y;

        }

        if (cube.position.z < - 3 || cube.position.z > 3) {

            cube.position.z = THREE.Math.clamp(cube.position.z, - 3, 3);
            cube.userData.velocity.z = - cube.userData.velocity.z;

        }

        cube.rotation.x += cube.userData.velocity.x * 2 * delta;
        cube.rotation.y += cube.userData.velocity.y * 2 * delta;
        cube.rotation.z += cube.userData.velocity.z * 2 * delta;

    }

    renderer.render(scene, camera);

}
