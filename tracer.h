#ifndef _NODE_ADDON_TRACER_H_
#define _NODE_ADDON_TRACER_H_


#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>
#include <node_version.h>
#include <node_buffer.h>
#include <string.h>
#include <nan.h>

#include <memory>
#include <functional>
#include <iterator>
#include <sstream>
#include <set>
#include "threadsafe_queue.h"
#include "uvasync.h"


#include "loglevels.h"


//register to node js engine
//unregister from node js (on exit)

//create logger
//delete logger
//check log messages in queue
//flush

//register logging callback from nodejs to library
//remove logging callback from nodejs



class tracer {
private:
	struct log_message
	{
		std::string module;
		LogLevel level;
		std::string message;
	};

	/*Logger Begin*/

	/*v8 logger callback pointer*/
	static Nan::Persistent<v8::Function> _logger;
	/*ffmpeg logger callback*/
	static void _ffmpeg_logger(std::string module, int level, std::string message);
	/*async callback for sending the messages on the main event loop*/
	static void _async_logger_callback(uv_async_t *handle/*, int status UNUSED*/);
	/*a queue for log messages*/
	static threadsafe_queue<log_message> _log_messages;
	/*uv async synchronizer for main event loop*/
	static std::shared_ptr<uvasync> _logger_async;

	/*Logger Ends*/

	static void flush_log_messages();

public:

	static void Init(v8::Handle<v8::Object> target);
	static void deinit(void*);
	static NAN_METHOD(RegisterLogger);
	static NAN_METHOD(Log);
	static NAN_METHOD(Flush);
	

	static NAN_SETTER(log_level_setter);
	static NAN_GETTER(log_level_getter);
	static LogLevel log_level;

	static NAN_SETTER(batch_length_setter);
	static NAN_GETTER(batch_length_getter);
	static unsigned int batch_length;

	static void Log(std::string module, LogLevel loglevel, std::string message);
	static void Log(std::string module, LogLevel loglevel, std::function<std::string()> message);

	template <typename T>
	static std::string join(const std::vector<T>& vec, std::function<std::string(T)> get, const char* delim) {
		std::string ret;
		for (auto &&v : vec) {
			if (v != *vec.begin()) {
				ret += delim;
			}
			ret += get(v);
		}
		return ret;
	}

	static std::string join(const std::vector<std::string>& vec, const char* delim);
	static std::string join(const std::set<std::string>& set, const char* delim);
};

#endif
