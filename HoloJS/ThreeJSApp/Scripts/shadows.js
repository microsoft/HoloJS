function ShadowsExample(scene, renderer) {
    this.scene = scene;
    this.renderer = renderer;

    function createLight(color) {

        var intensity = 1.5;

        var pointLight = new THREE.PointLight(color, intensity, 20);
        pointLight.castShadow = true;
        pointLight.shadow.camera.near = 1;
        pointLight.shadow.camera.far = 5;
        pointLight.shadow.bias = - 0.005; // reduces self-shadowing on double-sided objects

        var geometry = new THREE.SphereGeometry(0.03, 12, 6);
        var material = new THREE.MeshBasicMaterial({ color: color });
        material.color.multiplyScalar(intensity);
        var sphere = new THREE.Mesh(geometry, material);
        pointLight.add(sphere);

        var texture = new THREE.CanvasTexture(generateTexture());
        texture.magFilter = THREE.NearestFilter;
        texture.wrapT = THREE.RepeatWrapping;
        texture.wrapS = THREE.RepeatWrapping;
        texture.repeat.set(1, 3.5);

        var geometry = new THREE.SphereGeometry(0.25, 32, 8);
        var material = new THREE.MeshPhongMaterial({
            side: THREE.DoubleSide,
            alphaMap: texture,
            alphaTest: 0.5
        });

        var sphere = new THREE.Mesh(geometry, material);
        sphere.castShadow = true;
        sphere.receiveShadow = true;
        pointLight.add(sphere);

        // custom distance material
        var distanceMaterial = new THREE.MeshDistanceMaterial({
            alphaMap: material.alphaMap,
            alphaTest: material.alphaTest
        });
        sphere.customDistanceMaterial = distanceMaterial;

        return pointLight;

    }

    function generateTexture() {
        var canvas = document.createElement('canvas');
        canvas.width = 2;
        canvas.height = 2;

        var context = canvas.getContext('2d');
        context.fillStyle = '#ffffff';
        context.fillRect(0, 1, 2, 1);

        return canvas;
    }

    this.pointLight = createLight(0x0088ff);
    scene.add(this.pointLight);

    this.pointLight2 = createLight(0xff8888);
    scene.add(this.pointLight2);

    var geometry = new THREE.BoxGeometry(5, 5, 5);

    var material = new THREE.MeshPhongMaterial({
        color: 0xa0adaf,
        shininess: 10,
        specular: 0x111111,
        side: THREE.BackSide
    });

    var mesh = new THREE.Mesh(geometry, material);
    mesh.position.y = 1;
    mesh.receiveShadow = true;
    scene.add(mesh);

    renderer.shadowMap.enabled = true;
    renderer.shadowMap.type = THREE.BasicShadowMap;
    renderer.shadowMap.renderSingleSided = false; // must be set to false to honor double-sided materials

    this.update = function (delta, elapsed) {

        this.time = performance.now() * 0.001;

        this.pointLight.position.x = Math.sin(this.time * 0.6) * 1.8;
        this.pointLight.position.y = Math.sin(this.time * 0.7) * 1.8 + 1;
        this.pointLight.position.z = Math.sin(this.time * 0.8) * 1.8;

        this.pointLight.rotation.x = this.time;
        this.pointLight.rotation.z = this.time;

        this.time += 10000;

        this.pointLight2.position.x = Math.sin(this.time * 0.6) * 1.8;
        this.pointLight2.position.y = Math.sin(this.time * 0.7) * 1.8 + 1;
        this.pointLight2.position.z = Math.sin(this.time * 0.8) * 1.8;

        this.pointLight2.rotation.x = this.time;
        this.pointLight2.rotation.z = this.time;
    };
}