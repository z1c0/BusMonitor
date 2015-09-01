var getData = require('./getData');
var express = require('express');
var router = express.Router();


router.get('/text', function(req, res, next) {
  getData.fetch(function(o) { 
    var data = "";
    console.log(o);
    for (var t in o) {
      data += (t + ", " + o[t].direction + ", " + o[t].minutes + "\n");    
    }
    res.set('Content-Type', 'text/plain');
    res.send(data); 
  });
});

router.get('/json', function(req, res, next) {
  getData.fetch(function(o) { res.send(o); });
});

router.get('/', function(req, res, next) {
  getData.fetch(function(o) {
    console.log(o);
    res.render('index', { title: 'Froschberg Bus Monitor', times: o, year : new Date().getFullYear() });
  });
});

module.exports = router;
