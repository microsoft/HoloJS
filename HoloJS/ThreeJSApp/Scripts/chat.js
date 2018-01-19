function ChatWebsocketExample(scene, renderer) {

    var url = "https://socket-io-chat.now.sh";
    var socket = io(url, {});

    // Whenever the server emits 'new message', update the chat body
    socket.on('new message', function (data) {
        console.log(data.username + ":" + data.message);
    });

    // Whenever the server emits 'user joined', log it in the chat body
    socket.on('user joined', function (data) {
        console.log(data.username + ' joined');
    });

    socket.emit('add user', 'holojs-test');


    this.update = function (delta, elapsed) {
        
    };
}