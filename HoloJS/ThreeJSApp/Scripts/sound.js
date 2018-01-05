function SoundExample(scene, renderer) {
    let ambientLight = new THREE.AmbientLight(0xFFFFFF, 0.8);
    scene.add(ambientLight);

    let loader = new THREE.TextureLoader();
    let material = new THREE.MeshStandardMaterial({ vertexColors: THREE.VertexColors, map: new THREE.DataTexture(new Uint8Array(3).fill(255), 1, 1, THREE.RGBFormat) });

    // Add a cube that will be the source of the spatial sound
    let cube = new THREE.Mesh(new THREE.BoxBufferGeometry(0.2, 0.2, 0.2), material.clone());

    loader.setCrossOrigin('anonymous');
    material.map.needsUpdate = true;

    cube.position.set(0, 0, -1.5);
    cube.geometry.addAttribute('color', new THREE.BufferAttribute(Float32Array.from([
        1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, // right - red
        0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, // left - blue
        0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, // top - green
        1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, // bottom - yellow
        0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, // back - cyan
        1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0 // front - purple
    ]), 3));
    loader.load('texture.png', tex => { cube.material.map = tex; });

    scene.add(cube);

    this.update = function (delta, elapsed) {

    };

    //Create an AudioListener and add it to the camera
    var listener = new THREE.AudioListener();
    camera.add(listener);

    //Create the PositionalAudio object (passing in the listener)
    var sound = new THREE.PositionalAudio(listener);

    //Load a sound and set it as the PositionalAudio object's buffer
    var audioLoader = new THREE.AudioLoader();
    audioLoader.load('https://upload.wikimedia.org/wikipedia/en/4/45/ACDC_-_Back_In_Black-sample.ogg', function (buffer) {
        sound.setBuffer(buffer);
        sound.setRefDistance(20);
        sound.play();
    });

    //Finally add the sound to cube
    cube.add(sound);
}