const isHoloJs = typeof navigator.holojs !== 'undefined';
const canvas = document.createElement(isHoloJs ? 'canvasvr' : 'canvas');

if (isHoloJs === false) {
    // If running in browser, add the canvas to the DOM
    document.body.appendChild(canvas);
}

var camera, scene, renderer;
var controls;

var root;

var loader = new THREE.PDBLoader();
var offset = new THREE.Vector3();

init();

function init() {

    scene = new THREE.Scene();
    scene.background = new THREE.Color(0x050505);

    camera = new THREE.PerspectiveCamera(70, window.innerWidth / window.innerHeight, 0.25, 10);
    camera.position.z = 1000;
    scene.add(camera);

    var light = new THREE.DirectionalLight(0xffffff, 0.8);
    light.position.set(1, 1, 1);
    scene.add(light);

    var light = new THREE.DirectionalLight(0xffffff, 0.5);
    light.position.set(- 1, - 1, 1);
    scene.add(light);

    root = new THREE.Group();
    root.position.set(0, 1.5, -1.5);
    root.scale.set(0.002, 0.002, 0.002);
    scene.add(root);

    //

    renderer = new THREE.WebGLRenderer({ canvas, antialias: true });
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.setSize(window.innerWidth, window.innerHeight);

    renderer.setAnimationLoop(render);
    renderer.vr.enabled = true;

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

    loadMolecule('media/models/caffeine.pdb');
}

function loadMolecule(url) {

    while (root.children.length > 0) {

        var object = root.children[0];
        object.parent.remove(object);

    }

    loader.load(url, function (pdb) {

        var geometryAtoms = pdb.geometryAtoms;
        var geometryBonds = pdb.geometryBonds;
        var json = pdb.json;

        var boxGeometry = new THREE.BoxBufferGeometry(1, 1, 1);
        var sphereGeometry = new THREE.IcosahedronBufferGeometry(1, 2);

        geometryAtoms.computeBoundingBox();
        geometryAtoms.boundingBox.getCenter(offset).negate();

        geometryAtoms.translate(offset.x, offset.y, offset.z);
        geometryBonds.translate(offset.x, offset.y, offset.z);

        var positions = geometryAtoms.getAttribute('position');
        var colors = geometryAtoms.getAttribute('color');

        var position = new THREE.Vector3();
        var color = new THREE.Color();

        for (var i = 0; i < positions.count; i++) {

            position.x = positions.getX(i);
            position.y = positions.getY(i);
            position.z = positions.getZ(i);

            color.r = colors.getX(i);
            color.g = colors.getY(i);
            color.b = colors.getZ(i);

            let material = new THREE.MeshPhongMaterial({ color: color });

            let object = new THREE.Mesh(sphereGeometry, material);
            object.position.copy(position);
            object.position.multiplyScalar(75);
            object.scale.multiplyScalar(25);
            root.add(object);

            var atom = json.atoms[i];
        }

        positions = geometryBonds.getAttribute('position');

        var start = new THREE.Vector3();
        var end = new THREE.Vector3();

        for (var i = 0; i < positions.count; i += 2) {

            start.x = positions.getX(i);
            start.y = positions.getY(i);
            start.z = positions.getZ(i);

            end.x = positions.getX(i + 1);
            end.y = positions.getY(i + 1);
            end.z = positions.getZ(i + 1);

            start.multiplyScalar(75);
            end.multiplyScalar(75);

            let object = new THREE.Mesh(boxGeometry, new THREE.MeshPhongMaterial(0xffffff));
            object.position.copy(start);
            object.position.lerp(end, 0.5);
            object.scale.set(5, 5, start.distanceTo(end));
            object.lookAt(end);
            root.add(object);

        }
    });
}

//

function onWindowResize() {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
}

function render() {
    var time = Date.now() * 0.0004;

    root.rotation.x = time;
    root.rotation.y = time * 0.7;

    renderer.render(scene, camera);
}
