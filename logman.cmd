logman.exe start NodeEtw -p {10535C9F-BF2E-4A80-8410-EBBEBA2C7400} 0xff -o NativeNode.etl -ets
node test.js
logman.exe stop NodeEtw -ets