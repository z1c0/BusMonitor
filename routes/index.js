var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  
  var http = require('http');
    var options = {
      host: 'www.google.com',
      path: '/index.html'
    };
  
  var req2 = http.get(options, function(res2) {
    console.log('STATUS: ' + res.statusCode);
    console.log('HEADERS: ' + JSON.stringify(res.headers));
  
    // Buffer the body entirely for processing as a whole.
    var body = "";
    res2.on('data', function(chunk) {
      body += chunk;
    }).on('end', function() {
      console.log('BODY: ' + body);
      // ...and/or process the entire body here.
      res.render('index', { title: 'Express', data: body, year : new Date().getFullYear() });
    })
  });
  
  req2.on('error', function(e) {
    console.log('ERROR: ' + e.message);
  });  
});

module.exports = router;
