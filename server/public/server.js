"use strict"

import http from 'http';
import fs from 'fs';
import mime from mime;
import path from path;


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