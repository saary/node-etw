node-etw
========

A node [ETW](http://msdn.microsoft.com/en-us/library/windows/desktop/bb968803.aspx) logger that is a drop on replacement for console

usage
=======

```javascript
var etw = require('node-etw');
etw.log('My nice formated {%s}', 'line');
```

You will need to start an etw session (either realtime or to an etl file) in order to watch the logs.

For example:
```
logman.exe start NodeEtw -p {10535C9F-BF2E-4A80-8410-EBBEBA2C7400} 0xff -o NativeNode.etl -ets
node test.js
logman.exe stop NodeEtw -ets
```

###node ETW provider guid
{10535C9F-BF2E-4A80-8410-EBBEBA2C7400}

##License
MIT
