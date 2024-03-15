"use strict"

import mime from 'mime';
import http from 'http';
import fs from 'fs';
import path from 'path';


var server = http.createServer(function(request, response){
    var filePath = false;
    if(request.url == '/'){
        filePath = 'index.html';
    }else{
        filePath = '/' + request.url;
    }
    var absPath = './' + filePath;
    serveStatic(response,  absPath);
});

var PORT = 8080;
server.listen(PORT, function(){
    console.log("Server is listening on port " + PORT);
});

function serveStatic(response, absPath){
    fs.exists(absPath, function(exists){
        if(exists){
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
        {"content-type": mime.getType(path.basename(filePath))}
    );
    response.write(fileContents);
    response.end();
}

function noError(){
    document.getElementById('error-box').style.visibility = "hidden";
}

function connectError(){
    document.getElementById('error-box').style.visibility = "visible";
}

function updateVolume(){
    //get the volume via UDP/socket
    setVolume();
}