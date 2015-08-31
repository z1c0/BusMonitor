var getData = require('./getData');
var express = require('express');
var router = express.Router();


router.get('/text', function(req, res, next) {
  getData.fetch(function(o) { res.send(o); });
});

router.get('/', function(req, res, next) {
  getData.fetch(function(o) { 
    res.render('index', { title: 'Express', data: o, year : new Date().getFullYear() });
  });
});

module.exports = router;
