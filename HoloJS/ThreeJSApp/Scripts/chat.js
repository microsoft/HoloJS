// Chat client for the socket.io chat sample at https://socket.io/demos/chat/
function ChatWebsocketExample(scene, renderer) {
    var url = "https://socket-io-chat.now.sh";
    var socket = io(url, { transports: ['websocket'] });

    this.scene = scene;
    this.material = new THREE.MeshStandardMaterial({ vertexColors: THREE.VertexColors });
    scene.add(new THREE.AmbientLight(0xFFFFFF, 0.8));

    // Load fonts
    let fontLoader = new THREE.FontLoader();
    fontLoader.load('helvetiker_regular.typeface.json', function (font) {
        this.font = font;
    }.bind(this)); //end load function

    // Whenever the server emits 'new message', render the text
    socket.on('new message', function (data) {
        // Remove previous message
        if (this.textMesh) {
            scene.remove(this.textMesh);
            this.textMesh.geometry.dispose();
        }

        // If we have a font loaded, show the message
        if (this.font) {
            var textGeometry = new THREE.TextGeometry(data.username + ":" + data.message, {
                font: this.font,
                size: 0.2,
                height: 0.05,
                curveSegments: 12
            });

            this.textMesh = new THREE.Mesh(textGeometry, this.material);
            this.textMesh.position.z = -3;
            this.textMesh.position.x = -2;
            this.scene.add(this.textMesh);
        }

    }.bind(this));

    // Whenever the server emits 'user joined', log it
    socket.on('user joined', function (data) {
        console.log(data.username + ' joined');
    });

    // Join the chat
    socket.emit('add user', 'holojs-test');

    // Send a new message
    socket.emit('new message', "This is a new message");

    this.update = function (delta, elapsed) {    
    };
}