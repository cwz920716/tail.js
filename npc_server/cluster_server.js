var HOST = process.argv[2];
var PORT = process.argv[3];

var express = require('express');
var fs = require('fs');
var cluster = require('cluster');
var numCPUs = 8;

var contents = fs.readFileSync('index.html').toString();

if (cluster.isMaster) {
  for (var i = 0; i < numCPUs; i++) {
    cluster.fork();
  }
} else {
  var app = express();
  app.get('/', function (req, res) {
    res.send(contents);
  });

  var server = app.listen(PORT, HOST, function () {
    var host = server.address().address;
    var port = server.address().port;

    console.log('Example app listening at http://%s:%s', host, port);
  });
}
