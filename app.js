// https://github.com/voodootikigod/node-serialport

var express = require('express');
var bodyParser = require('body-parser');
var app = express();
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({extended:false}));

var serialport = require("serialport");
var SerialPort = serialport.SerialPort;
var serialPort = new SerialPort("/dev/cu.usbmodem14961", {
  baudrate: 9600,
  parser: serialport.parsers.readline("\n")
}, false); // do not open automagically

function sendCommandToArduino(command) {
  serialPort.open(function (error) {
    var serialData = '';
    if ( error ) {
      console.log('failed to open: '+error);
    } else {
      console.log('open');
      serialPort.on('data', function(data) {
        console.log('data received: ' + data);
      });

      serialPort.write(command + '\n', function(err, results) {
        if (err) {
          console.log('err ' + err);
        }
        console.log('results ' + results);
      });
    }
  });
}

function sendColorCommand(cmd, color) {
  sendCommandToArduino(cmd + ' ' + color.red + ' ' + color.green + ' ' + color.blue);
}

app.get('/status', function(req, res){
  sendCommandToArduino('status');
  res.json({succes:true});
});

app.post('/command', function(req, res) {
  var command = req.body;
  var action = command.action;
  var color = command.color;

  if (['on', 'off', 'theaterChaseRainbow', 'rainbow'].indexOf(action) != -1) {
    sendCommandToArduino(action);
    res.json({success: true});
  }

  if (['color', 'colorWipe', 'theaterChase'].indexOf(action) != -1 ) {
    sendColorCommand(action, color);
    res.json({success: true});
  }
});

console.log('awesome http server listening on port 8000');
app.listen(8000);
