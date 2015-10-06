// https://github.com/voodootikigod/node-serialport
var serialport = require('serialport');
var config = require('./config');

module.exports = function() {

  var SerialPort = serialport.SerialPort;
  var serialPort = new SerialPort(config.serialport, {
    baudrate: config.baudrate,
    parser: serialport.parsers.readline('\n')
  }, false); // do not open automagically

  var sendCommandToArduino = function(command, callback) {
    var responseList = [];
    serialPort.open(function(error) {
      if (error) {
        callback(error, null);

      } else {

        serialPort.on('data', function(data) {
          responseList.push(data);
        });

        serialPort.on('end', function() {
          console.log('end');
          callback(null, responseList);
        });

        serialPort.write(command + '\n', function(err, results) {
          callback(err, results);
        });
      }
    });
  };

  var sendColorCommand = function(cmd, color, callback) {
    var colorCmd = cmd + ' ' + color.red + ' ' + color.green + ' ' + color.blue;
    sendCommandToArduino(colorCmd, callback);
  };

  return {
    send: sendCommandToArduino,
    sendColor: sendColorCommand
  };

};
