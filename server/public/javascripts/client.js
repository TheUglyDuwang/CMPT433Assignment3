var socket = io.connect();
$(document).ready(function() {
	socket.on('vol', function(result) {
        console.log("volume = " + result);
        document.getElementById('volumeid').setAttribute('value', result);
	});
	socket.emit('fetchVol', "");
});

function sendCommandViaUDP(message) {
	socket.emit('daUdpCommand', message);
};

function update(){
    socket.emit('getVol');
    socket.on('vol', function(result){
        document.getElementById('volumeid').setAttribute('value', result);
    });
    socket.emit('getBPM');
    socket.on('bpm', function(result){
        document.getElementById('tempoid').setAttribute('value', result);
    })
}

setInterval(update(), 500);

function addVol(){
    socket.emit('changeVolume', 5);
    socket.on('newVol', function(result){
        document.getElementById('volumeid').setAttribute('value', result);
    });
}

function subVol(){
    socket.emit('changeVolume', -5);
    socket.on('newVol', function(result){
        document.getElementById('volumeid').setAttribute('value', result);
    });
}

function addTempo(){
    socket.emit('changeTempo', 5);
    socket.on('newBPM', function(result){
        document.getElementById('tempoid').setAttribute('value', result);
    })
}

function subTempo(){
    socket.emit('changeTempo', -5);
    socket.on('newBPM', function(result){
        document.getElementById('tempoid').setAttribute('value', result);
    })
}