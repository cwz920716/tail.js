var http = require("http"),
    url = require("url"),
    path = require("path"),
    fs = require("fs")
    port = process.argv[2] || 3000;
 
http.createServer(function(request, response) {
  var uri = url.parse(request.url).pathname
    , filename = path.join(process.cwd(), uri);
  
  fs.exists(filename, function(exists) {
    if(!exists) {
      response.writeHead(404, {"Content-Type": "text/plain"});
      response.write("404 Not Found\n");
      response.end();
      return;
    }
  });
 
  fs.readFile(filename, "binary", function(err, file) {
    response.writeHead(200);
    response.write(file, "binary");
    response.end();
  });

}).listen(parseInt(port, 10), "146.6.53.156");
 
console.log("Static file server running at\n  => http://localhost:" + port + "/\nCTRL + C to shutdown");

