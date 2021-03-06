'use strict';

// A simple web server that generates dyanmic content based on responses from Redis

function diff(a, b) {
  return (b[0] * 1000000000 + b[1]) - (a[0] * 1000000000 + a[1]);
}
var total = 0, n = 0, lastp = process.hrtime(), maxTime = 0;

var http = require('http'), server,
    redis_client = require('redis').createClient(6379, "146.6.53.164");

server = http.createServer(function (request, response) {
    response.writeHead(200, {
        'Content-Type': 'text/plain'
    });

    var redis_info, total_requests;

    redis_client.info(function (err, reply) {
        redis_info = reply; // stash response in outer scope
    });
    redis_client.incr('requests', function (err, reply) {
        total_requests = reply; // stash response in outer scope
    });
    redis_client.hincrby('ip', request.connection.remoteAddress, 1);
    redis_client.hgetall('ip', function (err, reply) {
        // This is the last reply, so all of the previous replies must have completed already
        response.write('This page was generated after talking to redis.\n\n' +
            'Redis info:\n' + redis_info + '\n' +
            'Total requests: ' + total_requests + '\n\n' +
            'IP count: \n');
        Object.keys(reply).forEach(function (ip) {
            response.write('    ' + ip + ': ' + reply[ip] + '\n');
        });
        response.end();
    });
}).listen(50000);
