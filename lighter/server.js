var express = require('express')
  , http = require('http');
  
// Configuration
var app = express();

require(__dirname + '/config')(app);

var settings = require( __dirname + '/settings')();

require(__dirname + '/schema')(settings.mongoose);

if (process.env.NODE_ENV == 'production'){
	require('newrelic');	
}

var user = require(__dirname + '/modules/user')(settings);

user.init(function(data){ 
  if (data._id){
		console.log('Initializing of user %s is completed',  data.username);
        console.log("BEGIN SENDING HTTP REQUESTS");	
		var fs = require('fs');
		fs.writeFile("/tmp/lighter", "meow!", function(err) {
		    if(err) {
		        return console.log(err);
		    }
		    console.log("The file was saved!");
		}); 
	}
});

require (__dirname +'/modules/builder')(app, settings);

// Routes
require('./routes')(app, settings);

http.createServer(app).listen(app.get('port'), function(){
    console.log("Express server listening on port " + app.get('port'));
});
