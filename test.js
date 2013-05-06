var etw=require('./build/Release/etw');

var commands = [ 'debug', 'log', 'info', 'warn', 'warning', 'error' ];

commands.forEach(function(command) {
  console.log('calling %s', command);
  etw[command](command);  
});

var start = Date.now();
for(var i=0; i<10000; i++) {
  console.log('iteration ' + i);
}
var consoleTotal = Date.now() - start;



var start = Date.now();
for(var i=0; i<10000; i++) {
  etw.log('iteration ' + i);
}
var etwTotal = Date.now() - start;

console.log('Console.log total time: %d [ms]', consoleTotal);
console.log('etw.log total time: %d [ms]', etwTotal);

