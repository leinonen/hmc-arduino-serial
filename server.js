var express = require('express');
var bodyParser = require('body-parser');
var app = express();
var config = require('./config');
var NeoPixel = require('./neopixel');
var neopixel = new NeoPixel();

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({extended: false}));

app.get('/status', function(req, res) {
  neopixel.send('status', function(err, data) {
    if (err) {
      res.json({success: false, error: err});
    } else {
      res.json({success: true, data: data})
    }
  });
});

function isValidCommand(action) {
  return ['on', 'off', 'theaterChaseRainbow', 'rainbow'].indexOf(action) != -1;
}

function isValidColorCommand(action) {
  return ['color', 'colorWipe', 'theaterChase'].indexOf(action) != -1
}

app.post('/command', function(req, res) {
  var command = req.body;
  var action = command.action;
  var color = command.color;

  if (isValidCommand(action)) {
    neopixel.send(action, function(err, data) {
      if (err) {
        res.json({success: false, error: err});
      } else {
        res.json({success: true, data: data})
      }
    });

  } else if (isValidColorCommand(action)) {
    neopixel.sendColor(action, color, function(err, data) {
      if (err) {
        res.json({success: false, error: err});
      } else {
        res.json({success: true, data: data})
      }
    });

  } else {
    res.json({success: false, error: 'invalid action: ' + action});
  }
});

console.log('hmc-neopixel http server listening on port %d', config.port);
app.listen(config.port);
