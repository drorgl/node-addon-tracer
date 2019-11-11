/**
 * Log Level Enumeration
 *
 * @export
 * @enum {number}
 */
export enum LogLevel {
    TRACE = 0,	
    DEBUG = 1,	
    INFO = 2,
    WARN = 3,
    ERROR =4,	
    FATAL = 5
}

/**
 * Log Message Callback Interface
 *
 * @export
 * @interface ILogCallback
 */
export interface ILogCallback {
    (module: string, logLevel: LogLevel, message: string): void;
}

export interface ITracer {

    /**
     * Registers a Javascript Logging Callback to the addon logger
     * Only one can be registered, a 2nd call will unregister the previous callback
     *
     * @param {ILogCallback} loggerCallback
     * @memberof ITracer
     */
    RegisterLogger(loggerCallback: ILogCallback): void;
    /**
     * Flush will force a logging callback flush
     * This is to avoid waiting for the next event loop cycle
     *
     * @memberof ITracer
     */
    Flush(): void;

    /**
     * maximum log messages in queue before old log messages are being overridden
     *
     * @type {number}
     * @memberof ITracer
     */
    buffer_length: number;
    /**
     * maximum number of log messages to pass to v8 in each batch
     * a larger number might hold the v8 engine for longer time between 
     * intervals and can hurt performance, use a high value only if you're missing 
     * log messages
     *
     * @type {number}
     * @memberof ITracer
     */
    batch_length: number;

    /**
     * log level
     * this value will determine if the log will be 
     * queued or not, not if the callback will return a certain level
     *
     * @type {LogLevel}
     * @memberof ITracer
     */
    log_level: LogLevel;

    /**
     * Force a log into the log queue, this function is a 
     * waste of CPU cycles and should be used only for testing
     *
     * @param {string} module
     * @param {LogLevel} logLevel
     * @param {string} message
     * @memberof ITracer
     */
    Log(module: string, logLevel: LogLevel, message: string): void;
}

