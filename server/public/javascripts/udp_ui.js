var socket = io.connect();
$(document).ready(function() {
	socket.on('vol', function(result) {
        document.getElementById('volumeid').setAttribute('value', result);
	});
    socket.on('bpm', function(result){
        document.getElementById('tempoid').setAttribute('value', result);
    });
	socket.emit('getVol', "");
});

function sendCommandViaUDP(message) {
	socket.emit('daUdpCommand', message);
};

function update(){
    socket.emit('getVol');
    socket.emit('getBPM');
}

setInterval(update(), 100);

function addVol(){
    socket.emit('changeVolume', 5);
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