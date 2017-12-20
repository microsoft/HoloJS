function ShadowsExample(scene, renderer) {

    var light = new THREE.DirectionalLight(0xffffff, 1);
    light.position.set(0, 5, 0);
    light.castShadow = true;
    scene.add(light);
    var helper = new THREE.CameraHelper(light.shadow.camera);
    scene.add(helper);

    var material = new THREE.MeshPhongMaterial({
        color: 0xa0adaf,
        shininess: 10,
        specular: 0x111111,
        side: THREE.BackSide
    });

    let big_sphere = new THREE.Mesh(new THREE.SphereBufferGeometry(0.5, 20, 20), material.clone());
    big_sphere.position.set(0, 0, 0);
    big_sphere.material.color.set(0xff0000);
    big_sphere.material.roughness = 0.3;
    big_sphere.material.metalness = 0.2;
    big_sphere.castShadow = true;
    big_sphere.receiveShadow = true;
    scene.add(big_sphere);

    let small_sphere = new THREE.Mesh(new THREE.SphereBufferGeometry(0.1, 20, 20), material.clone());
    small_sphere.position.set(0, 2, 0);
    small_sphere.material.color.set(0xff0000);
    small_sphere.material.roughness = 0.3;
    small_sphere.material.metalness = 0.2;
    small_sphere.castShadow = true;
    small_sphere.receiveShadow = true;
    scene.add(small_sphere);

    renderer.shadowMap.enabled = true;
    renderer.shadowMap.type = THREE.BasicShadowMap;

    this.update = function (delta, elapsed) {
    };
}