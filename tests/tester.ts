

import * as tracer from "../tracer"

console.log("starting");
import tape = require("tape");
import chalk = require("chalk");
import util = require("util");


let errorColor = chalk.red.bold;
let okColor = chalk.green.bold;
var level = 0;

function logger(module, level, message) {
    //if (level < 99) {
    //console.log(module, level, message);
    //}
}
function tablevel() {
    let retval = "";
    for (let i = 0; i < level; i++) {
        retval += "\t";
    }
    return retval;
}

var results = {
    passed: 0,
    failed: 0
};

var tapestream = tape.createStream({ objectMode: true });

tapestream.on('data', (row) => {
    //console.log(JSON.stringify(row));
    if (typeof row == typeof "") {
        console.log(tablevel() + row);
    }
    else if (row.type == "end") {
        console.log();
        level--;
    }
    else if (row.type == "test") {
        level++;
        console.log();
        console.log(tablevel() + "%d. Testing %s", row.id, row.name);
    }
    else {
        if (row.ok) {
            results.passed++;
            console.log(tablevel() + okColor("%d. \t %s \t %s"), row.id, row.ok, row.name);
            if (row.operator == "throws" && row.actual != undefined) {
                console.log(tablevel() + okColor(" threw: %s"), row.actual);
            }
        }
        else {
            results.failed++;
            console.log(tablevel() + errorColor("%d. \t %s \t %s"), row.id, row.ok, row.name);
            console.log(tablevel() + errorColor("\t expected: %s actual: %s"), row.expected, row.actual);
        }
    }
});

tapestream.on('end', (r) => {
    console.log("passed:", results.passed);
    console.log("failed:", results.failed);
});



function showObject(tobj) {
    let objstr = JSON.stringify(tobj, null, '\t');
    let showObjectContents = false;
    if (showObjectContents) {
        console.log(objstr);
    }
    else {
        console.log("object size: " + objstr.length);
    }
}

interface TesterTracer extends tracer.ITracer {
    LogTrace(module: string, message: string): void;
    LogDebug(module: string, message: string): void;
    LogInfo(module: string, message: string): void;
    LogWarn(module: string, message: string): void;
    LogError(module: string, message: string): void;
    LogFatal(module: string, message: string): void;
}

var addon: TesterTracer = null;
var aoRelease = null;
var aoDebug = null;

if (addon == null) {
    try {
        addon = require('../build/Release/tracer-tester');
    } catch (e) {
        aoRelease = e;
    }
}

if (addon == null) {
    try {
        addon = require('../build/Debug/tracer-tester');
    } catch (e) {
        aoDebug = e;
    }
}

if (addon == null) {
    console.log("failed to load addon");
    console.log("release:", aoRelease);
    console.log("debug:", aoDebug);
}

function timeme(func: Function) : string {
    let startTime = process.hrtime();
    func();
    let endTime = process.hrtime(startTime);
    return util.format("%d seconds, %d ms", endTime[0], endTime[1] / 1000000);
}




tape('check custom logger in tester.cc', function (t) {
    let numberOfLogMessages = 6;



    addon.RegisterLogger((module, logLevel, message) => {
        //console.log("log", module, logLevel, message);
        numberOfLogMessages--;
    });//, "module", tracer.LogLevel.TRACE);
    let time_for_logging = timeme(() => {
        addon.LogDebug("test", "message");
        addon.LogTrace("test", "message");
        addon.LogInfo("test", "message");
        addon.LogWarn("test", "message");
        addon.LogError("test", "message");
        addon.LogFatal("test", "message");
    });

    t.comment("logging time " +  time_for_logging);

    let startTime = process.hrtime();

    let checkFinished = () => {
        if (numberOfLogMessages == 0) {
            let endTime = process.hrtime(startTime);
            t.comment(util.format("start to finish took %d seconds and %d ms", endTime[0], endTime[1] / 1000000));
            t.pass("6 messages received");
            t.end();
        } else {
            setImmediate(checkFinished);
        }
    };
    checkFinished();
});


tape('check custom logger in tester.cc filter Warnings and up', function (t) {
    let numberOfLogMessages = 3;



    addon.RegisterLogger((module, logLevel, message) => {
        //console.log("log", module, logLevel, message);
        numberOfLogMessages--;
    });//, "module", tracer.LogLevel.TRACE);
    let time_for_logging = timeme(() => {
        addon.log_level = tracer.LogLevel.WARN;
        addon.LogDebug("test", "message");
        addon.LogTrace("test", "message");
        addon.LogInfo("test", "message");
        addon.LogWarn("test", "message");
        addon.LogError("test", "message");
        addon.LogFatal("test", "message");
    });

    t.comment("logging time " + time_for_logging);

    let startTime = process.hrtime();

    let checkFinished = () => {
        if (numberOfLogMessages == 0) {
            let endTime = process.hrtime(startTime);
            t.comment(util.format("start to finish took %d seconds and %d ms", endTime[0], endTime[1] / 1000000));
            t.pass("3 messages received");
            t.end();
        } else {
            setImmediate(checkFinished);
        }
    };
    checkFinished();
});

tape('logger without flushing', function (t) {
    let numberOfLogMessages = 1000;
    let logMessagesLeft = numberOfLogMessages;



    addon.RegisterLogger((module, logLevel, message) => {
        //console.log("log", module, logLevel, message);
        logMessagesLeft--;
    });//, "module", tracer.LogLevel.TRACE);
    let time_for_logging = timeme(() => {
        for (let i = 0; i < numberOfLogMessages; i++) {
            addon.Log("test", tracer.LogLevel.ERROR, "message");
        }
    });

    t.comment("logging time " + time_for_logging);

    let startTime = process.hrtime();

    let checkFinished = () => {
        if (logMessagesLeft == 0) {
            let endTime = process.hrtime(startTime);
            t.comment(util.format("start to finish took %d seconds and %d ms", endTime[0], endTime[1] / 1000000));
            t.pass("1000 messages received");
            t.end();
        } else {
            setImmediate(checkFinished);
        }
    };
    checkFinished();
});

tape('logger with flushing', function (t) {
    let numberOfLogMessages = 1000;
    let logMessagesLeft = numberOfLogMessages;



    addon.RegisterLogger((module, logLevel, message) => {
        //console.log("log", module, logLevel, message);
        logMessagesLeft--;
    });//, "module", tracer.LogLevel.TRACE);
    let time_for_logging = timeme(() => {
        for (let i = 0; i < numberOfLogMessages; i++) {
            addon.Log("test", tracer.LogLevel.ERROR, "message");
            addon.Flush();
        }
    });

    t.comment("logging time " + time_for_logging);

    let startTime = process.hrtime();

    let checkFinished = () => {
        if (logMessagesLeft == 0) {
            let endTime = process.hrtime(startTime);
            t.comment(util.format("start to finish took %d seconds and %d ms", endTime[0], endTime[1] / 1000000));
            t.pass("1000 messages received");
            t.end();
        } else {
            setImmediate(checkFinished);
        }
    };
    checkFinished();
});



tape('logger with small batch length', function (t) {
    let numberOfLogMessages = 1000;
    addon.batch_length = 1;
    let logMessagesLeft = numberOfLogMessages;



    addon.RegisterLogger((module, logLevel, message) => {
        //console.log("log", module, logLevel, message);
        logMessagesLeft--;
    });//, "module", tracer.LogLevel.TRACE);
    let time_for_logging = timeme(() => {
        for (let i = 0; i < numberOfLogMessages; i++) {
            addon.Log("test", tracer.LogLevel.ERROR, "message");
            addon.Flush();
        }
    });

    t.comment("logging time " + time_for_logging);

    let startTime = process.hrtime();

    let checkFinished = () => {
        if (logMessagesLeft == 0) {
            let endTime = process.hrtime(startTime);
            t.comment(util.format("start to finish took %d seconds and %d ms", endTime[0], endTime[1] / 1000000));
            t.pass("1000 messages received");
            t.end();
        } else {
            setImmediate(checkFinished);
        }
    };
    checkFinished();
});
