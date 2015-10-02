var express = require('express');
var bodyParser = require('body-parser');
var app = express();
var config = require('./config');
var NeoPixel = require('./neopixel');
var neopixel = new NeoPixel();

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({extended:false}));

app.get('/status', function(req, res){
  sendCommandToArduino('status');
  res.json({succes:true});
});

app.post('/command', function(req, res) {
  var command = req.body;
  var action = command.action;
  var color = command.color;

  if (['on', 'off', 'theaterChaseRainbow', 'rainbow'].indexOf(action) != -1) {
    neopixel.sendCommandToArduino(action);
    res.json({success: true});
  }

  if (['color', 'colorWipe', 'theaterChase'].indexOf(action) != -1 ) {
    neopixel.sendColorCommand(action, color);
    res.json({success: true});
  }
});

console.log('hmc-neopixel http server listening on port %d', config.port);
app.listen(config.port);
