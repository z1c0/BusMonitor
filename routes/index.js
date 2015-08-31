var express = require('express');
var router = express.Router();

function getData(f) {
  var http = require('http');
    var options = {
      host: 'www.google.com',
      path: '/index.html'
    };
  
  var req = http.get(options, function(res) {
    //console.log('STATUS: ' + res.statusCode);
    //console.log('HEADERS: ' + JSON.stringify(res.headers));
  
    var body = "";
    res.on('data', function(chunk) {
      body += chunk;
    }).on('end', function() {
      //console.log('BODY: ' + body);
      f(body);
    })
  });  
  req.on('error', function(e) {
    console.log('ERROR: ' + e.message);
  });  
}




router.get('/text', function(req, res, next) {
  getData(function(o) { res.send(o); });
});

router.get('/', function(req, res, next) {
  getData(function(o) { 
    res.render('index', { title: 'Express', data: o, year : new Date().getFullYear() });
  });
});

module.exports = router;
