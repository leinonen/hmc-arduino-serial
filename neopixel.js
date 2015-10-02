// https://github.com/voodootikigod/node-serialport
var serialport = require('serialport');
var config = require('./config');

module.exports = function() {

  var SerialPort = serialport.SerialPort;
  var serialPort = new SerialPort(config.serialport, {
    baudrate: config.baudrate,
    parser: serialport.parsers.readline('\n')
  }, false); // do not open automagically

  function sendCommandToArduino(command) {
    serialPort.open(function(error) {
      var serialData = '';
      if (error) {
        console.log('failed to open: ' + error);
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

  return {
    sendColorCommand: sendColorCommand
  };

};
