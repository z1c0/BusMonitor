var http = require('http');
var parseString = require('xml2js').parseString;


var doRequest = function(callback, urlParams, renderCallback) {
  var options = {
    host: 'www.linzag.at',
    path: '/static/XML_DM_REQUEST?'
  };
  options.path += urlParams;
  var req = http.request(options);
  req.on('response', function(res) {
    var data = '';
    // concat chunks
    res.on('data', function(chunk) { data += chunk });  
    // when the response has finished
    res.on('end', function() {          
      //console log(data);
      parseString(data, function (err, result) {
        // fire callback      
        callback(result, renderCallback);
      });
    });
  });
  req.end();
}

function filter(line, direction)
{
  return (line == "27" && direction == "Linz Chemiepark") ||
         (line == "46" && direction == "Linz Hafen") ||
         (line == "26" && direction == "Linz St. Margarethen");
}

function getTimes(result, renderCallback) {
  var departures = result.itdRequest.itdDepartureMonitorRequest[0].itdDepartureList[0].itdDeparture;
  //console.log(departures);
  var nextDepartures = {}; 
  for (var i in departures) {
    var d = departures[i];
    var lineNr = d.itdServingLine[0].$.number;
    var direction = d.itdServingLine[0].$.direction;
    var minutes = d.$.countdown;
    //console.log("Linie " + lineNr + " (" + direction + "): " + minutes + " min.");
    if (filter(lineNr, direction)) {
      var id = lineNr + " " + direction;
      if (id in nextDepartures) {
        minutes = Math.min(minutes, nextDepartures[id]);
      } 
      nextDepartures[id] = minutes;
    }
  }
  //console.log(nextDepartures);
  renderCallback(JSON.stringify(nextDepartures));
}

function getLines(result, renderCallback) { 
  var sessionId = result.itdRequest.$.sessionID;
  var requestId = result.itdRequest.itdDepartureMonitorRequest[0].$.requestID; 
  var lines = result.itdRequest.itdDepartureMonitorRequest[0].itdServingLines[0].itdServingLine; 
  //console.log(sessionId);
  //console.log(requestId);
  //console.log(lines.length);
  var urlParams = "sessionID=" + sessionId + "&requestID=" + requestId;
  for (var i in lines) {
    var line = lines[i].$
    //console.log("[" + line.index + "] " + line.number + ": " + line.direction);
    urlParams += ("&dmLineSelection=" + line.index);
  }  
  doRequest(getTimes, urlParams, renderCallback);
}

module.exports = {
  fetch: function (f) {
    doRequest(getLines, "sessionID=0&type_dm=any&name_dm=60500560", f);
  }
};
