import socketio from 'socket.io'


var volume = 80;
var bpm = 120;

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

var io;
exports.listen = function(server){
    io = socketio.listen(server);
    io.sockets.on('connection', function(socket){
        handleCommand(socket);
    });
}

function handleCommand(socket){
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
    });
}