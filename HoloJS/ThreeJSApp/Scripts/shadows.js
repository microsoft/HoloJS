function ShadowsExample(scene, renderer) {

    renderer.shadowMap.enabled = true;
    renderer.shadowMap.type = THREE.PCFSoftShadowMap;

    spotLight = new THREE.SpotLight(0xffffff, 1);
    spotLight.position.set(1, 2, 3);
    spotLight.angle = Math.PI / 4;
    spotLight.penumbra = 0.05;
    spotLight.decay = 0.2;
    spotLight.distance = 10;

    spotLight.castShadow = true;
    spotLight.shadow.mapSize.width = 1024;
    spotLight.shadow.mapSize.height = 1024;
    spotLight.shadow.camera.near = 1;
    spotLight.shadow.camera.far = 10;
    scene.add(spotLight);

    var material = new THREE.MeshPhongMaterial({ color: 0x808080, dithering: true });
    var geometry = new THREE.BoxGeometry(10, 0.1, 10);

    var mesh = new THREE.Mesh(geometry, material);
    mesh.position.set(0, -1, 0);
    mesh.receiveShadow = true;
    scene.add(mesh);

    var material = new THREE.MeshPhongMaterial({ color: 0x4080ff, dithering: true });
    var geometry = new THREE.BoxGeometry(0.3, 0.1, 0.2);
    var mesh = new THREE.Mesh(geometry, material);
    mesh.position.set(-0.5, -0.5, -1);
    mesh.castShadow = true;
    scene.add(mesh);

    var ambient = new THREE.AmbientLight(0xffffff, 0.1);
    scene.add(ambient);
}