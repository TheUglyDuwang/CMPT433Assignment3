"use strict"

import mime from 'mime';
import http from 'http';
import fs from 'fs';
import path from 'path';

var volume = 80;
var bpm = 120;

var server = http.createServer(function(request, response){
    var filePath = false;
    if(request.url == '/'){
        filePath = 'stylesheets/index.html';
    }else{
        filePath = 'stylesheets' + request.url;
    }
    var absPath = './' + filePath;
    serveStatic(response,  absPath);
});

var PORT = 3042;
server.listen(PORT, function(){
    console.log("Server is listening on port " + PORT);
});

function serveStatic(response, absPath){
    fs.existsSync(absPath, function(existsSync){
        if(existsSync){
            fs.readFile(absPath, function(err, data){
                if(err){
                    send404(response);
                }else{
                    sendFile(response, absPath, data);
                }
            });
        }else{
            send404(response);
        }
    });
}

function send404(response){
    response.writeHead(404, {'Content-Type': 'text/plain'});
    response.write('Error 404: resource not found');
    response.end();
}

function sendFile(response, filePath, fileContents){
    response.writeHead(200,
        {"content-type": mime.lookup(path.basename(filePath))}
    );
    response.end(fileContents);
}

function noError(){
    document.getElementById('error-box').style.visibility = "hidden";
}

function connectError(){
    document.getElementById('error-box').style.visibility = "visible";
}

function incrementVolume(){
    volume = volume + 5;
    if(volume > 100){
        volume = 100;
    }
    document.getElementById("volumeid").innerHTML = volume;
}

function decrimentVolume(){
    volume = volume - 5;
    if(volume < 0){
        volume = 0;
    }
    document.getElementById("volumeid").innerHTML = volume;
}

function incrementBPM(){
    bpm = bpm + 5;
    if(volume > 300){
        bpm = 300;
    }
    //document.getElementById("volumeid").innerHTML = volume;
}

function decrimentBPM(){
    bpm = bpm - 5;
    if(volume < 40){
        bpm = 40;
    }
    //document.getElementById("volumeid").innerHTML = volume;
}