var etw = require(__dirname + '/build/Release/etw');
var util = require('util');
var methods = [ 'debug', 'log', 'info', 'warn', 'warning', 'error' ];

methods.forEach(function(method) {
  exports[method] = function() {
    var str = util.format.apply(null, arguments);    
    return etw[method](str);
  };
});