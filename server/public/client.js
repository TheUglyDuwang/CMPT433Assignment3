var volume = 80;
var bpm = 120;

function incrementVolume(){
    volume = volume + 5;
    if(volume > 100){
        volume = 100;
    }
    document.getElementById("volumeid").setAttribute('value', volume);
}

function decrimentVolume(){
    volume = volume - 5;
    if(volume < 0){
        volume = 0;
    }
    document.getElementById("volumeid").setAttribute('value', volume);
}

function incrementBPM(){
    bpm = bpm + 5;
    if(bpm > 300){
        bpm = 300;
    }
    document.getElementById("tempoid").setAttribute('value', bpm);
}

function decrimentBPM(){
    bpm = bpm - 5;
    if(bpm < 40){
        bpm = 40;
    }
    document.getElementById("tempoid").setAttribute('value', bpm);
}

function getVolume(){
    return volume;
}

function setVolume(val){
    volume = val;
}