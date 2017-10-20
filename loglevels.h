#ifndef _TRACER_LOGLEVELS_H_
#define _TRACER_LOGLEVELS_H_

namespace node_addon_tracer {

#ifdef ERROR
	static const int _errorValue = ERROR;
#undef ERROR
	static const int ERROR = _errorValue;
#define ERROR ERROR
#endif

#ifdef DEBUG
#undef DEBUG
#define DEBUG DEBUG
#endif 

	enum class LogLevel : uint8_t {
		TRACE = 0,
		DEBUG = 1,
		INFO = 2,
		WARN = 3,
		ERROR = 4,
		FATAL = 5
	};

}

#endif