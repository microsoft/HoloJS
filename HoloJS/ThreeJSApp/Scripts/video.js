function VideoTextureExample(scene, renderer) {
    let canvas = document.createElement('canvas');
    canvas.width = 480;
    canvas.height = 204;

    var video = document.createElement("video");
    video.src = "http://techslides.com/demos/sample-videos/small.mp4";
    video.autoplay = true;

    videoImageContext = canvas.getContext('2d');
    videoImageContext.fillStyle = '#FFFFFF';
    videoImageContext.fillRect(0, 0, canvas.width, canvas.height);

    let texture = new THREE.VideoTexture(video);
    texture.format = THREE.RGBAFormat;
    texture.minFilter = THREE.LinearFilter;
    texture.magFilter = THREE.LinearFilter;

    let canvasCube = new THREE.Mesh(
        new THREE.PlaneGeometry(0.5, 0.5),
        new THREE.MeshStandardMaterial({ side: THREE.DoubleSide, map: texture, overdraw: 0.5 })
    );

    canvasCube.position.set(0, 0, -1.5);
    scene.add(canvasCube);

    var ambient = new THREE.AmbientLight(0xffffff, 1);
    scene.add(ambient);
}