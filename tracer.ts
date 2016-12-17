export enum LogLevel {
    TRACE = 0,	
    DEBUG = 1,	
    INFO = 2,
    WARN = 3,
    ERROR =4,	
    FATAL = 5
}

export interface ILogCallback {
    (module: string, logLevel: LogLevel, message: string): void;
}

export interface ITracer {
    RegisterLogger(loggerCallback: ILogCallback): void;//, module?: string, logLevel?: LogLevel)
    Flush(): void;
    UnregisterCallback(loggerCallback: ILogCallback): void;

    //bufferLength: number; //maximum log messages in queue before old log messages are being overriden
    batch_length: number; //maximum number of log messages to pass to v8 in each batch

    log_level: LogLevel;

    Log(module: string, logLevel: LogLevel, message: string): void;
}

