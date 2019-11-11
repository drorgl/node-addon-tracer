# node addon tracer
Linux: [![Travis CI Build Status](https://api.travis-ci.org/drorgl/node-addon-tracer.svg)](https://travis-ci.org/drorgl/node-addon-tracer)
Windows 32/64/VS2013/VS2015: [![Build status](https://ci.appveyor.com/api/projects/status/4wu1d7hcpu8k7j6x?svg=true)](https://ci.appveyor.com/project/drorgl/node-addon-tracer)

## node-addon-tracer

This module solves a problem where node addons are not supplied with tracing library.

## Usage
See tester.cc

Initialize the module:
```C++
tracer::Init(target);
```

To Log messages:
```C++
static void Log(std::string&& module, LogLevel loglevel, std::string&& message);
```


Inside your TypeScript code, you can attach to the logger:
```typescript
import * as tracer from "tracer"
tracer.RegisterLogger(module, logLevel, message) => {
        console.log("log", module, logLevel, message);
});
```

Remove the logger callback:
```typescript
tracer.RegisterLogger();
```

Flush a pending batch log messages:
```typescript
tracer.Flush();
```

Set the maximum amount of messages that will be queued:
```typescript
tracer.buffer_length = 100;
```

Set a logging callback batch length:
```typescript
tracer.batch_length = 10;
```

Modify the log level for the logger:
```typescript
tracer.log_level = LogLevel.DEBUG;
```

or test the whole addon logging works:
```typescript
tracer.Log("testModule", LogLevel.DEBUG, "hello world");
```

### License
MIT License

Copyright (c) 2016 Dror Gluska

See LICENSE file
