var socketio = require('socket.io');

var dgram = require('dgram');

var volume = 80;
var bpm = 120;

var io;
exports.listen = function(server){
    io = socketio.listen(server);
    io.sockets.on('connection', function(socket){
        handleCommand(socket);
    });
}

function handleCommand(socket){

    socket.on('fetchVol', function(data){
        socket.emit('vol', volume);
    });

	socket.on('daUdpCommand', function(data) {
		console.log('daUdpCommand command: ' + data);

		// Info for connecting to the local process via UDP
		var PORT = 12345;
		var HOST = '127.0.0.1';
		var buffer = new Buffer(data);

		var client = dgram.createSocket('udp4');
		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
			if (err) 
				throw err;
			console.log('UDP message sent to ' + HOST +':'+ PORT);
		});
        client.on('listening', function () {
			var address = client.address();
			console.log('UDP Client: listening on ' + address.address + ":" + address.port);
		});
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {
			console.log("UDP Client: message Rx" + remote.address + ':' + remote.port +' - ' + message);

			var reply = message.toString('utf8')
			socket.emit('commandReply', reply);

			client.close();

		});
		client.on("UDP Client: close", function() {
			console.log("closed");
		});
		client.on("UDP Client: error", function(err) {
			console.log("error: ",err);
		});
    });


/*
    socket.on('changeVolume', function(changeAmount){
        if(changeAmount > 0){
            incrementVolume();
        }else{
            decrimentVolume();
        }
        socket.emit('newVol', volume);
    });
    socket.on('changeTempo', function(changeAmount){
        if(changeAmount > 0){
            incrementBPM();
        }else{
            decrimentBPM();
        }
        socket.emit('newBPM', volume);
    });
    socket.on('getVol', function(){
        socket.emit('vol', volume);
    });
    socket.on('getBPM', function(){
        socket.emit('bpm', bpm);
    });*/
}

function incrementVolume(){
    volume = volume + 5;
    if(volume > 100){
        volume = 100;
    }
}

function decrimentVolume(){
    volume = volume - 5;
    if(volume < 0){
        volume = 0;
    }
}

function incrementBPM(){
    bpm = bpm + 5;
    if(bpm > 300){
        bpm = 300;
    }
}

function decrimentBPM(){
    bpm = bpm - 5;
    if(bpm < 40){
        bpm = 40;
    }
}