'use strict';

// A simple web server that generates dyanmic content based on responses from Redis

function diff(a, b) {
  return (b[0] * 1000000000 + b[1]) - (a[0] * 1000000000 + a[1]);
}
var total = 0, n = 0, lastp = process.hrtime(), maxTime = 0;

var http = require('http'), server,
    redis_client = require('redis').createClient();

server = http.createServer(function (request, response) {
    var beforeTime = process.hrtime();
    var redis_info, total_requests;

    var r = Math.random();
    redis_client.hgetall('ip', function (err, reply) {

        // This is the last reply, so all of the previous replies must have completed already
        response.writeHead(200, {
          'Content-Type': 'text/plain'
        });
        response.write('This page was generated after talking to redis.\n\n' +
            'Redis info:\n' + redis_info + '\n' +
            'Total requests: ' + total_requests + '\n\n' +
            'IP count: \n');
        Object.keys(reply).forEach(function (ip) {
            response.write('    ' + ip + ': ' + reply[ip] + '\n');
        });

      var afterTime = process.hrtime();
      var diff2 = diff(beforeTime, afterTime);
      total += diff2;
      if (diff2 > maxTime) maxTime = diff2;
      n++;
      if (diff(lastp, afterTime) > 1e9 * 10) {
        console.log('avg redis IO = ' + (total / n) + ' n = ' + n);
        console.log('max redis IO = ' + maxTime + ' n = ' + n);
        maxTime = 0;
        lastp = afterTime;
        
      }
        response.end();
    });
}).listen(50000);
