function LightsExample(scene, renderer) {

    let material = new THREE.MeshPhysicalMaterial({ color: 0x00ff00, roughness: 0.5, metalness: 1.0 });

    let sphere = new THREE.Mesh(initColors(new THREE.SphereBufferGeometry(0.1, 20, 20)), material.clone());
    sphere.position.set(0.4, 0, -1.5);
    sphere.material.color.set(0xff0000);
    sphere.material.roughness = 0.3;
    sphere.material.metalness = 0.2;
    scene.add(sphere);

    let cone = new THREE.Mesh(initColors(new THREE.ConeBufferGeometry(0.1, 0.2, 20, 20)), material.clone());
    cone.position.set(-0.4, 0, -1.5);
    cone.material.color.set(0x0000ff);
    cone.material.roughness = 0.5;
    scene.add(cone);


    let torus = new THREE.Mesh(initColors(new THREE.TorusKnotBufferGeometry(0.2, 0.02, 100, 100)), material.clone());
    torus.scale.set(1.5, 1.5, 1.5);
    torus.material.color.set(0x00ff00);
    torus.material.roughness = 0.5;
    torus.material.metalness = 1.0;
    scene.add(torus);

    let directionalLight = new THREE.DirectionalLight(0xFFFFFF, 0.5);
    directionalLight.position.set(0, 2, 0);
    scene.add(directionalLight);

    let ambientLight = new THREE.AmbientLight(0xFFFFFF, 0.8);
    scene.add(ambientLight);
    
    this.pointLight = new THREE.PointLight(0xFFFFFF, 0.5);
    scene.add(this.pointLight);

    function initColors(geometry) {
        return geometry.addAttribute('color', new THREE.BufferAttribute(new Float32Array(geometry.attributes.position.array.length).fill(1.0), 3));
    }

    this.update = function (delta, elapsed) {
        this.pointLight.position.set(0 + 2.0 * Math.cos(elapsed * 0.5), 0, -1.5 + 2.0 * Math.sin(elapsed * 0.5));
    };
}