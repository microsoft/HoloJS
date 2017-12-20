function CanvasRenderingExample(scene, renderer) {

    let canvas = document.createElement('canvas');
    canvas.width = 300;
    canvas.height = 300;
    let ctx = canvas.getContext('2d');

    // Draw rectangle
    ctx.fillStyle = 'rgba(255, 0, 0, 0.5)';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Draw text
    ctx.font = '40px Arial';
    ctx.fillStyle = '#00ff00';
    ctx.fillText('Hello\nCanvas', 10, 10);

    // Draw gradient
    let gradient = ctx.createLinearGradient(0, 0, 100, 0);
    gradient.addColorStop(0, 'rgb(0, 255, 0)');
    gradient.addColorStop(1, 'rgb(0, 0, 255)');
    ctx.fillStyle = gradient;
    ctx.fillRect(10, 150, 100, 50);

    // Draw Image
    let image = new Image();
    image.src = 'image.png';
    image.onload = () => {
        ctx.drawImage(image, 150, 150, 100, 100);
        texture.needsUpdate = true;
    };

    let texture = new THREE.Texture(canvas);
    texture.minFilter = THREE.LinearFilter;
    texture.needsUpdate = true;

    let canvasCube = new THREE.Mesh(
        new THREE.PlaneGeometry(0.2, 0.2),
        new THREE.MeshStandardMaterial({
            side: THREE.DoubleSide,
            map: texture,
            transparent: true
        })
    );

    canvasCube.position.set(0, 0.3, -1.5);
    scene.add(canvasCube);

    this.update = function() {

    }
}