const isHoloJs = typeof navigator.holojs !== 'undefined';
const canvas = document.createElement(isHoloJs ? 'canvasvr' : 'canvas');

if (isHoloJs === false) {
    // If running in browser, add the canvas to the DOM
    document.body.appendChild(canvas);
}

var scene, camera, renderer;

function init() {

    camera = new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.1, 100);
    camera.position.set(3, 2, 3);

    var reflectionCube = new THREE.CubeTextureLoader()
        .setPath('media/textures/cube/SwedishRoyalCastle/')
        .load(['px.jpg', 'nx.jpg', 'py.jpg', 'ny.jpg', 'pz.jpg', 'nz.jpg']);
    reflectionCube.format = THREE.RGBFormat;

    scene = new THREE.Scene();
    scene.background = new THREE.Color(0xa0a0a0);
    scene.fog = new THREE.Fog(0xa0a0a0, 2, 20);

    //

    var hemiLight = new THREE.HemisphereLight(0xffffff, 0x444444);
    hemiLight.position.set(0, 20, 0);
    scene.add(hemiLight);

    var dirLight = new THREE.DirectionalLight(0xffffff);
    dirLight.position.set(5, 5, 0);
    dirLight.castShadow = true;
    dirLight.shadow.camera.top = 1;
    dirLight.shadow.camera.bottom = - 1;
    dirLight.shadow.camera.left = - 1;
    dirLight.shadow.camera.right = 1;
    dirLight.shadow.camera.near = 0.1;
    dirLight.shadow.camera.far = 20;
    scene.add(dirLight);

    //

    var mesh = new THREE.Mesh(new THREE.PlaneBufferGeometry(50, 50), new THREE.MeshPhongMaterial({ color: 0x999999, depthWrite: false }));
    mesh.rotation.x = - Math.PI / 2;
    mesh.receiveShadow = true;
    scene.add(mesh);

    var grid = new THREE.GridHelper(10, 10, 0x888888, 0x888888);
    scene.add(grid);

    //

    var listener = new THREE.AudioListener();
    camera.add(listener);

    var positionalAudio = new THREE.PositionalAudio(listener);

    var audioLoader = new THREE.AudioLoader();
    audioLoader.load('media/sounds/376737_Skullbeatz___Bad_Cat_Maste.mp3', function (buffer) {
        positionalAudio.setBuffer(buffer);
        positionalAudio.setRefDistance(1);
        positionalAudio.setDirectionalCone(180, 230, 0.1);
        positionalAudio.play();

        let helper = new THREE.PositionalAudioHelper(positionalAudio, 0.1);
        positionalAudio.add(helper);
    });

    //

    var gltfLoader = new THREE.GLTFLoader();
    gltfLoader.load('media/models/gltf/BoomBox.glb', function (gltf) {

        var boomBox = gltf.scene;
        boomBox.position.set(0, 0.2, 0);
        boomBox.scale.set(20, 20, 20);

        boomBox.traverse(function (object) {

            if (object.isMesh) {

                object.material.envMap = reflectionCube;
                object.geometry.rotateY(- Math.PI);
                object.castShadow = true;

            }

        });

        boomBox.add(positionalAudio);
        scene.add(boomBox);
        animate();

    });

    // sound is damped behind this wall

    var wallGeometry = new THREE.BoxBufferGeometry(2, 1, 0.1);
    var wallMaterial = new THREE.MeshBasicMaterial({ color: 0xff0000, transparent: true, opacity: 0.5 });

    var wall = new THREE.Mesh(wallGeometry, wallMaterial);
    wall.position.set(0, 0.5, - 0.5);
    scene.add(wall);


    //

    renderer = new THREE.WebGLRenderer({ canvas, antialias: true });
    renderer.setSize(window.innerWidth, window.innerHeight);
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.gammaOutput = true;
    renderer.gammaFactor = 2.2;
    renderer.shadowMap.enabled = true;

    //

    if (isHoloJs) {
        controls = new THREE.OrbitControls(camera, window);
    } else {
        controls = new THREE.OrbitControls(camera, canvas);
    }
    controls.target.set(0, 0.1, 0);
    controls.update();
    controls.minDistance = 0.5;
    controls.maxDistance = 10;
    controls.maxPolarAngle = 0.5 * Math.PI;

    //

    window.addEventListener('resize', onWindowResize, false);

}

function onWindowResize() {

    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();

    renderer.setSize(window.innerWidth, window.innerHeight);

}

function animate() {
    renderer.setAnimationLoop(render);
    if (isHoloJs === false) {
        renderer.vr.enabled = true;
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
    renderer.render(scene, camera);
}

init();